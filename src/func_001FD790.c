// NEARMISS func_001FD790  (vram 0x001FD790, 0x1C0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 57.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order swap: target colors idx->$s2 / frame->$s0, mwcc colors idx->$s0 / frame->$s2, offsetting nearly the whole function by one slot; plus reversed switch dispatch order on D_008106F5. Pure regalloc/dispatch-order permutation class -- permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Frame/clip stepping loop. Each iteration: arg0+0x34 = arg0+8 + arg0+0x60
// (accumulated frame), idx = low 31 bits of that value. If bit31 is set, the
// entry pointer is D_00264DD0[0]; otherwise it is D_00264DD0[D_00810700+1].
// entry = base + idx*8; flags = entry[5] (byte), frame = entry[0] (half). When
// bit31 is clear there is a state-machine gate on D_008106F5 (and whether
// arg0+0x60 advanced): mode 2 returns 0; for the not-yet-advanced path with
// mode 0 and flags==0 it tries func_001FD580(idx, arg0+0x70) and returns 0 on
// success. Common tail: if both flags and frame are zero, increments arg0+0x60
// and loops again when frame is still zero; otherwise commits frame to
// arg0+0x6C (zeroing arg0+0x68 when arg0+0x60 is 0), writes flags to arg0+0x50
// and entry[4] to arg0+0x51, and returns 1.
//
// Residual (dominant): a register-allocation-order swap -- the target colors
// idx -> $s2 and frame -> $s0, while mwcc colors idx -> $s0 and frame -> $s2,
// which offsets nearly every instruction by one slot. Secondary: the switch on
// D_008106F5 dispatches in reverse case order under mwcc. Both are pure
// regalloc/dispatch-order permutations -- permuter territory; idioms exhausted.
extern int func_001FD580(unsigned long long idx, int p);
extern int D_00264DD0[];
extern unsigned char D_008106F5;
extern unsigned char D_00810700;

int func_001FD790(int arg0) {
    int v;
    unsigned long long idx;
    int base;
    int entry;
    int flags;
    unsigned int frame;

    for (;;) {
        v = *(int *)(arg0 + 8) + *(int *)(arg0 + 0x60);
        *(int *)(arg0 + 0x34) = v;
        v = *(int *)(arg0 + 0x34);
        idx = ((unsigned long long)v << 33) >> 33;
        if (v & 0x80000000) {
            base = D_00264DD0[0];
            entry = base + idx * 8;
            flags = *(unsigned char *)(entry + 5);
            frame = *(unsigned short *)(entry + 0);
        } else {
            base = D_00264DD0[D_00810700 + 1];
            entry = base + idx * 8;
            flags = *(unsigned char *)(entry + 5);
            frame = *(unsigned short *)(entry + 0);
            if (*(int *)(arg0 + 0x60) != 0) {
                switch (D_008106F5) {
                case 2:
                    return 0;
                case 1:
                case 0:
                    break;
                default:
                    break;
                }
                goto common;
            }
            switch (D_008106F5) {
            case 0:
                if ((flags & 0xFF) != 0) {
                    goto common;
                }
                if (func_001FD580(idx, arg0 + 0x70) == 1) {
                    return 0;
                }
                goto common;
            case 2:
                return 0;
            default:
                break;
            }
        }
    common:
        if ((flags & 0xFF) == 0 && (frame & 0xFFFF) == 0) {
            *(int *)(arg0 + 0x60) = *(int *)(arg0 + 0x60) + 1;
        }
        if ((flags & 0xFF) == 0) {
            if ((frame & 0xFFFF) == 0) {
                continue;
            }
        }
        v = frame & 0xFFFF;
        if (*(int *)(arg0 + 0x60) == 0) {
            *(int *)(arg0 + 0x68) = 0;
            v = frame & 0xFFFF;
        }
        *(int *)(arg0 + 0x6C) = v;
        *(unsigned char *)(arg0 + 0x50) = flags;
        *(unsigned char *)(arg0 + 0x51) = *(unsigned char *)((int)idx * 8 + base + 4);
        return 1;
    }
}
