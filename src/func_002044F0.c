// NEARMISS func_002044F0  (vram 0x002044F0, 0x204 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.40% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation near-miss. Logic fully recovered & body byte-identical (fixed the 64-bit compose to a 28-bit mask: (s0 & 0x0FFFFFFF) | 0x30000000, not sign-extend; made the do-while pre-guard slt via 0<v0). EVERY remaining diff is a register-name mismatch: target keeps arg0 in s2 and loop tem...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Ring-buffer / DMA-chain submit guarded by a semaphore. SignalSema, then if the
// arg0[0x44] flag is clear just queue func_00204080 and bail; otherwise advance the
// modular read/write indices (arg0[2]=size, [3]/[4]=head/tail, [5] packs count<<11 |
// frac) and issue func_002041A0 transfers (mode 3, prio 0x80), the last one mode 0.
// Finally re-arm via CreateSema. The s0 compose keeps low 28 bits | 0x30000000 |0x100
// (a tag/qword-count word) when a wrap occurred.
// NEARMISS 92.4% (mwcc 2.3.3): logic fully recovered & body byte-identical; the sole
// residual is whole-function register allocation - target uses caller-saved t-regs
// for loop temps + frame 0x40, mwcc spills to callee-saved s-regs + frame 0x60.
extern int SignalSema(int);
extern int CreateSema(int);
extern void func_00204080(void *);
extern int func_002040A0(int *, int);
extern void func_00204140(int);
extern int func_002041A0(int, int, int, int);
extern int D_00273470;

int func_002044F0(int *arg0) {
    int s0;
    int t0;
    int t1;
    int v0;
    int s1;
    int hi;
    int a2;

    s1 = 0;
    SignalSema(arg0[0x40 / 4]);
    if (arg0[0x44 / 4] == 0) {
        func_00204080(&D_00273470);
        return 0;
    }
    func_00204140(5);
    s0 = *(int *)0x1000B400;
    hi = (func_002040A0(arg0, *(int *)0x1000B410) + arg0[2] - arg0[3]) % arg0[2];
    arg0[3] = (arg0[3] + hi) % arg0[2];
    arg0[4] -= hi;
    t0 = (arg0[3] + arg0[4]) % arg0[2];
    v0 = arg0[5] >> 0xB;
    arg0[5] = arg0[5] % 2048;
    if (v0 > 0) {
        hi = (arg0[2] + (arg0[3] + arg0[4] - 1)) % arg0[2];
        v0 = func_002041A0(arg0[1] + hi * 0x10, arg0[0] + (hi << 0xB), 3, 0x80);
        s1 = 1;
    }
    t1 = 0;
    if (0 < v0) {
        do {
            a2 = (t1 == v0 - 1) ? 0 : 3;
            v0 = func_002041A0(arg0[1] + t0 * 0x10, arg0[0] + (t0 << 0xB), a2, 0x80);
            t1 += 1;
            t0 = (t0 + 1) % arg0[2];
        } while (t1 < v0);
    }
    arg0[4] += v0;
    if (arg0[4] != 0) {
        int a0 = s0 | 0x100;
        if (s1 != 0) {
            s0 = (s0 & 0x0FFFFFFF) | 0x30000000;
            a0 = s0 | 0x100;
        }
        func_00204140(a0);
    }
    return CreateSema(arg0[0x40 / 4]);
}
