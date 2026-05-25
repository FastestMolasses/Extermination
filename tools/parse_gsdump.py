#!/usr/bin/env python3
"""Minimal PCSX2 GS dump parser.

Layout (new format, magic 0xFFFFFFFF):
  u32 magic = 0xFFFFFFFF
  u32 header_size
  GSDumpHeader { u32 state_version, state_size, serial_offset, serial_size,
                 crc, ssw, ssh, ss_offset, ss_size }   (rest of header is padding/screenshot)
  u8[state_size] state_data   (GS freeze blob)
  u8[8192]      regs_data     (GSPrivRegSet PMODE..)
  packets:
    u8 id
      id==0 Transfer: u8 path, u32 length, u8[length] data
      id==1 VSync:    u8 field
      id==2 ReadFIFO2: u32 size
      id==3 Registers: u8[8192]
"""

import struct, sys, os

PATH_NAMES = {0: "PATH1old", 1: "PATH2", 2: "PATH3", 3: "PATH1new", 4: "Dummy"}

def parse(path):
    with open(path, "rb") as f:
        buf = f.read()
    o = 0
    magic, = struct.unpack_from("<I", buf, o); o += 4
    assert magic == 0xFFFFFFFF, f"unexpected magic {magic:#x}"
    header_size, = struct.unpack_from("<I", buf, o); o += 4
    hdr = buf[o:o+header_size]
    state_version, state_size, serial_off, serial_sz, crc, ssw, ssh, ss_off, ss_sz = \
        struct.unpack_from("<9I", hdr, 0)
    serial = hdr[serial_off:serial_off+serial_sz].decode("ascii", "replace")
    o += header_size
    state_data = buf[o:o+state_size]; o += state_size
    regs = buf[o:o+8192]; o += 8192
    print(f"magic OK; serial={serial!r} crc={crc:08x}")
    print(f"state_size={state_size} ({state_size:#x})  screenshot={ssw}x{ssh} ss_size={ss_sz}")
    print(f"state_data at file offset {8+header_size}..{8+header_size+state_size}")
    print(f"packets begin at {o} ({o:#x}), file size {len(buf)}")

    transfer_counts = {0:0,1:0,2:0,3:0,4:0}
    transfer_bytes  = {0:0,1:0,2:0,3:0,4:0}
    vsyncs = 0
    pkts = []
    while o < len(buf):
        pid = buf[o]; o += 1
        if pid == 0:  # Transfer
            ppath = buf[o]; o += 1
            plen,  = struct.unpack_from("<I", buf, o); o += 4
            data = buf[o:o+plen]; o += plen
            transfer_counts[ppath] += 1
            transfer_bytes[ppath]  += plen
            pkts.append(("T", ppath, plen, data))
        elif pid == 1:  # VSync
            field = buf[o]; o += 1
            vsyncs += 1
            pkts.append(("V", field))
        elif pid == 2:  # ReadFIFO2
            sz, = struct.unpack_from("<I", buf, o); o += 4
            pkts.append(("R", sz))
        elif pid == 3:  # Registers
            o += 8192
            pkts.append(("G",))
        else:
            print(f"unknown packet id {pid} at {o-1}; stopping")
            break

    print(f"vsyncs in capture: {vsyncs}")
    for k, n in transfer_counts.items():
        if n:
            print(f"  path {PATH_NAMES[k]} ({k}): {n} packets, {transfer_bytes[k]} bytes")
    return state_data, regs, pkts, serial, crc

if __name__ == "__main__":
    parse(sys.argv[1])
