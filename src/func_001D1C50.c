// NEARMISS func_001D1C50  (vram 0x001D1C50, 0x248 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.96% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 95.96% mwcc233 near-miss (logic fully recovered). Residual = register coloring in the D_00275670 struct-write tail (target t0/t1 vs mwcc a4/a5 for the 0xB0 long-long + reloaded base), one commutative addu operand order, and one delay-slot nop in the func_001B0070 branch. sdata handled (gp-rel 275...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 95.96% (mwcc233). Logic fully recovered. Residuals are register
// allocation/coloring in the D_00275670 struct-write tail: mwcc233 colors the
// 0xB0 long-long temp and reloaded base pointer into t0/t1 (shown a3/a4/a5 by the
// disassembler) differently than the target, plus one commutative addu operand
// order and one delay-slot nop in the func_001B0070 branch. Not source-driveable
// (tried explicit long-long temp, load reordering, per-access reload idiom like
// sibling func_001D1F20). D_00275670/674 are gp-rel small globals (threshold 4);
// the 0x00810xxx / 0x70003xxx globals are declared as unsized arrays so mwcc keeps
// them out of small-data and addresses them absolutely, matching the target.
// Behavior: func_001D2830(4,0) startup, then a mode dispatch (D_008106C4 flag /
// func_001B0070 bit 0x80 / else path) that runs func_0021B970/9A0/BA80 with fixed
// float params and func_0015D2F0/D_008106C6 gating, then seeds a display-list slot
// off D_00275670: copies the 0xB0 qword to slot 0x360, writes the 0x10 command
// pointer (byte3=0x30, +4=off, halfword=3, advance +0x10), issues func_001D2960,
// copies two 4-qword blocks (D_70003A40/AC0) and finalizes via func_001D7C30/30A0.
extern unsigned char D_70003B8D;                    /* PS2 scratchpad @ 0x70003B8D */

extern void func_001D2830(int, int);
extern int func_0015D2F0(void);
extern int func_001B0070(void);
extern void func_001D2960(void *, int, int, int);
extern void func_001D30A0(void);
extern void func_001D7C30(void);
extern void func_0021B970(float, float);
extern void func_0021B9A0(float, float, int);
extern void func_0021BA80(int, int, int);
extern void copy_qw4(void *, char *);
extern char *D_00275670;
extern char *D_00275674;
extern char D_00810610[];
extern unsigned char D_008106C4[];
extern unsigned char D_008106C6[];
extern unsigned char D_008106C7[];
extern unsigned char D_00810700[];
extern char D_70003A40[];
extern char D_70003AC0[];

void func_001D1C50(void) {
    char *p;
    char *b;
    int idx;
    int off;
    int flag;

    func_001D2830(4, 0);
    if (D_008106C4[0] != 0) {
        func_001D2830(6, 0);
    } else if (func_001B0070() & 0x80) {
        if (D_008106C7[0] == 0) {
            flag = 0;
            if ((func_0015D2F0() == 2) && (D_008106C6[0] == 2)) {
                flag = 1;
            }
            if (flag == 0) {
                func_0021B970(0.0f, 50.0f);
                goto skip;
            }
        }
        if (D_00810700[0] == 8) {
            func_0021B970(50.0f, 150.0f);
        } else {
            func_0021B970(0.0f, 210.0f);
        }
    skip:
        func_0021BA80(8, 8, 0x15);
        func_001D2830(6, 1);
    } else {
        flag = 0;
        if ((D_70003B8D == 0) || (D_70003B8D == 4)) {
            if ((func_0015D2F0() == 2) && (D_008106C6[0] == 2)) {
                flag = 1;
            }
            if (flag == 0) {
                func_0021B9A0(0.0f, 0.0f, 0);
            }
        }
        func_001D2830(6, 1);
    }
    p = D_00275670;
    b = D_00275674;
    idx = *(int *)(p + 0x9C);
    *(long long *)(b + idx * 0x30 + 0x360) = *(long long *)(p + 0xB0);
    p = D_00275670;
    b = D_00275674;
    idx = *(int *)(p + 0x9C);
    (*(char **)(p + 0x10))[3] = 0x30;
    off = (int)(b + idx * 0x30) + 0x340;
    *(int *)(*(char **)(p + 0x10) + 4) = off;
    *(short *)(*(char **)(p + 0x10)) = 3;
    *(char **)(p + 0x10) = *(char **)(p + 0x10) + 0x10;
    func_001D2960(D_00810610, off, (int)b, idx * 2);
    copy_qw4(D_70003A40, D_00275670 + 0x2340);
    copy_qw4(D_70003AC0, D_00275670 + 0x23C0);
    func_001D7C30();
    func_001D30A0();
}
