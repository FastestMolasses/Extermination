"""Measurement harness: drive single original FPU / VU0-macro instructions in PCSX2.

Instruction instances are taken from the original boot ELF (addresses only);
operands are written through the DebugServer, the instruction is single-stepped,
and the destination is read back. Nothing is saved; the emulator is terminated
at the end so the save state on disk is untouched (pcsx2_session hashes it).
"""
import json, socket, struct, sys
from pathlib import Path
REPO = Path(__file__).resolve().parents[2]  # decomp repo root
DATA = REPO / 'build/startup-reference/ee_float'  # recorded vectors (oracle input, ignored)
sys.path.insert(0, str(REPO / 'tools'))
from pcsx2_session import OriginalSession, SSTATES

ELF = (REPO / 'config/SCUS_971.12').read_bytes()
HERE = Path(__file__).resolve().parent
STATE = SSTATES / 'SCUS-97112 (0AE679AF).04.p2s'
CAT = {'GPR': 0, 'FPR': 2, 'FCR': 3, 'VU0F': 4, 'VU0I': 5}


def word(va):
    return struct.unpack_from('<I', ELF, va - 0x100000 + 0x300)[0]


def h128(lanes):
    return '0x' + ''.join('%08x' % (x & 0xffffffff) for x in reversed(lanes))


class Link:
    """One persistent DebugServer connection; commands may be pipelined."""
    def __init__(self, port=21512):
        self.sock = socket.create_connection(('127.0.0.1', port), timeout=10)
        self.buf = b''

    def batch(self, cmds):
        self.sock.sendall(b''.join((json.dumps(c) + '\n').encode() for c in cmds))
        out = []
        while len(out) < len(cmds):
            while b'\n' not in self.buf:
                chunk = self.sock.recv(1 << 16)
                if not chunk:
                    raise EOFError('DebugServer closed')
                self.buf += chunk
            line, self.buf = self.buf.split(b'\n', 1)
            r = json.loads(line)
            if not r.get('ok'):
                raise RuntimeError(r)
            out.append(r)
        return out

    def close(self):
        self.sock.close()


def parse_regs(resp):
    (block,) = [v for v in resp['data'].values() if isinstance(v, dict)]
    out = []
    for r in block['regs']:
        v = r['value'].replace('0x', '').rjust(32, '0')
        out.append([int(v[24 - 8 * i:32 - 8 * i], 16) for i in range(4)])
    return out


def W(cat, idx, value):
    if isinstance(value, (list, tuple)):
        v = h128(value)
    else:
        v = '0x%08x' % (value & 0xffffffff)
    return {'cmd': 'write_register', 'category': CAT[cat], 'index': idx, 'value': v}


def STEP(va):
    return [{'cmd': 'set_pc', 'value': va}, {'cmd': 'step'}]


def READ(cat):
    return {'cmd': 'read_registers', 'category': CAT[cat]}


class Rig:
    def __init__(self, s):
        self.s = s
        self.link = Link()
        self.steps = 0

    def do(self, cmds):
        """Run a pipelined command list; check every step landed on pc+4; return parsed reads."""
        rs = self.link.batch(cmds)
        reads = []
        for c, r in zip(cmds, rs):
            if c['cmd'] == 'step':
                self.steps += 1
                if int(r['new_pc'], 16) != int(r['old_pc'], 16) + 4:
                    raise RuntimeError(f'step {r["old_pc"]} landed at {r["new_pc"]}')
            elif c['cmd'] == 'read_registers':
                reads.append(parse_regs(r))
        return reads


def session():
    return OriginalSession(STATE, log_dir=REPO / 'build/ee_float/session')
