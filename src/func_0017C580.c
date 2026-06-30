// NEARMISS func_0017C580  (vram 0x0017C580, 0x2D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 98.78% near-miss, logic fully recovered. 2 residual instrs: (1) register-allocation permutation (0x63 compare value a0 vs target a2); (2) branch-delay scheduling permutation -- mwcc 2.3.3 fills the (0x228>=100.0f) bc1t delay slot with the speculative 0x70003A20 float reload where the 2.3.1 target...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 98.78% (logic fully recovered). Actor fall/landing state handler:
// d = field(0xB4)-field(0x2F4), stashed at scratch float 0x70003A20; picks a
// landing reaction (state byte +6) from thresholds (-104 hard, -50, -14.5).
// The c==0x63 case and the func_001000E0(...)&&(+0x234==1) case share one
// reset block. func_00182870 takes (actor,1); func_001FBD50/func_001749A0
// take a trailing float arg (300.0f / 1.0f). D_008106F1 array-decl forces
// non-gp-rel addressing.
// RESIDUAL WALL (2 instrs): (1) the c==0x63 compare value colored a0 vs the
// target's a2 (register-allocation permutation); (2) mwcc 2.3.3 fills the
// (0x228>=100.0f) bc1t delay slot with the speculative 0x70003A20 reload
// where the 2.3.1 target leaves a nop (branch-delay scheduling permutation).
extern int func_001000E0(int, int);
extern int func_00128350(float);
extern void func_001749A0(char *, int, int, float);
extern void func_00174AC0(char *, int);
extern int func_001755B0(char *);
extern void func_00182870(char *, int);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(char *, int, int, float);
extern void func_0021C350(char *);

extern unsigned char D_008106F1[16];

void func_0017C580(char *arg0) {
    float d;

    *(char *)(arg0 + 5) = 8;
    *(char *)(arg0 + 6) = 0;
    *(char *)(arg0 + 0x1F0) = 0xF;
    func_00182870(arg0, 1);
    *(char *)(arg0 + 0x25C) = 0;
    *(int *)(arg0 + 0x38) = 0;
    if (*(unsigned char *)(arg0 + 0xF) == 0x63) {
        goto reset;
    }
    if (func_001000E0(func_00128350(*(float *)(arg0 + 0x220)), 0) != 0 &&
        *(unsigned char *)(arg0 + 0x234) == 1) {
reset:
        *(char *)(arg0 + 0) = 2;
        *(char *)(arg0 + 4) = 2;
        *(char *)(arg0 + 5) = 3;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x3F;
        return;
    }
    d = *(float *)(arg0 + 0xB4) - *(float *)(arg0 + 0x2F4);
    *(float *)0x70003A20 = d;
    if (d < -104.0f) {
        *(char *)(arg0 + 6) = 1;
        func_001B61C0(1, 0xEE, 0x3C, 1);
        *(char *)(arg0 + 0) = 2;
        *(float *)(arg0 + 0x220) = 0.0f;
        *(char *)(arg0 + 0x25F) = 0;
        func_001FBD50(arg0, 0x151, 0, 300.0f);
        func_001749A0(arg0, 0x2B, 0, 1.0f);
        *(char *)(arg0 + 0x1F0) = 0x40;
    } else {
        func_00174AC0(arg0, 0);
        if (*(float *)(arg0 + 0x220) <= 0.0f) {
            *(char *)(arg0 + 6) = 3;
        } else if (*(float *)(arg0 + 0x228) >= 100.0f && D_008106F1[0] != 0) {
            *(char *)(arg0 + 6) = 0xA;
        } else {
            d = *(float *)0x70003A20;
            if (d <= -50.0f) {
                if (*(unsigned char *)(arg0 + 0x23F) == 3 && func_001755B0(arg0) == 0) {
                    *(char *)(arg0 + 6) = 2;
                    func_001FBD50(arg0, 0x13D, 0, 300.0f);
                } else {
                    *(char *)(arg0 + 6) = 3;
                    func_001B61C0(0, 0xD0, 0xA, 1);
                    *(int *)(arg0 + 0x224) = 0x40A00000;
                    func_0021C350(arg0);
                    func_001FBD50(arg0, 0x151, 0, 300.0f);
                }
            } else if (d <= -14.5f) {
                if (*(unsigned char *)(arg0 + 0x23F) == 3 && func_001755B0(arg0) == 0) {
                    *(char *)(arg0 + 6) = 2;
                    func_001FBD50(arg0, 0x13D, 0, 300.0f);
                } else {
                    *(char *)(arg0 + 6) = 4;
                }
            } else {
                *(char *)(arg0 + 6) = 5;
            }
        }
    }
    *(char *)(arg0 + 7) = 0;
}
