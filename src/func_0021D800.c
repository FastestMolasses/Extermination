// NEARMISS func_0021D800  (vram 0x0021D800, 0x3B0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling wall: mwcc233 re-materializes 'paddub a0,s0,zero' before the func_0021D600 calls where the target keeps a0 live from the previous call (saved-reg-arg-in-jal-delay-slot class); residual branch-offset cascade from that + one more insertion in the epilogue region. Body...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern int func_00122BB8(void);
extern void func_001749A0(char *a0, int a1, int a2, float f12);
extern int func_00175900(char *p, int a);
extern void func_00178B90(char *p, int a);
extern void func_00179880(char *a0, float *a1);
extern void func_0017C540(char *p, int flags);
extern void func_001B61C0(int a0, int a1, int a2, int a3);
extern void func_001FBD50(char *a0, int a1, int a2, float f12);
extern int func_0021D1A0(char *a0);
extern void func_0021D250(unsigned char *p, int a1);
extern void func_0021D530(char *arg0);
extern int func_0021D600(char *a0);
extern char **D_00275B40;

void func_0021D800(char *arg0) {
    unsigned char st;
    float fade;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg0 + 7) = 0;
        *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
        func_001B61C0(0, 0xC0, 5, 1);
        if (*(unsigned char *)(arg0 + 0x1F1) == 1) {
            func_001FBD50(arg0, 0x153, 0, 300.0f);
        } else {
            func_001FBD50(arg0, 0x152, 0, 300.0f);
        }
        if (func_00122BB8() & 1) {
            if (*(unsigned char *)(arg0 + 0x236) != 0) {
                func_001749A0(arg0, 0x56, 0, 0.0f);
            } else if (*(unsigned char *)(arg0 + 0x234) == 0) {
                if (func_0021D1A0(arg0) != 0) {
                    func_001749A0(arg0, 0x1E, 0, 0.0f);
                } else {
                    func_001749A0(arg0, 0x1F, 0, 0.0f);
                }
            } else {
                func_001749A0(arg0, 0x1C7, 0, 0.0f);
            }
            if (func_0021D600(arg0) == 0) {
                func_001FBD50(arg0, 0x146, 0, 300.0f);
            }
        } else {
            if (*(unsigned char *)(arg0 + 0x236) != 0) {
                func_001749A0(arg0, 0x57, 0, 0.0f);
            } else if (*(unsigned char *)(arg0 + 0x234) == 0) {
                if (func_0021D1A0(arg0) != 0) {
                    func_001749A0(arg0, 0x20, 0, 0.0f);
                } else {
                    func_001749A0(arg0, 0x21, 0, 0.0f);
                }
            } else {
                func_001749A0(arg0, 0x1C7, 0, 0.0f);
            }
            if (func_0021D600(arg0) == 0) {
                func_001FBD50(arg0, 0x147, 0, 300.0f);
            }
        }
        *(float *)(arg0 + 0x38) = 0.0f;
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(int *)(arg0 + 0x2EC) = 0;
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            if (*(unsigned char *)(arg0 + 0x1F1) == 2) {
                *(short *)(arg0 + 0x20E) = 0x5A;
            } else {
                *(short *)(arg0 + 0x20E) = 0x3C;
            }
            if (*(unsigned char *)(arg0 + 0x319) != 0 && *(unsigned char *)(arg0 + 5) == 0x17) {
                func_0021D530(arg0);
                return;
            }
            fade = -0.2f;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + fade;
            func_00179880((char *)arg0, (float *)(arg0 + 0x2EC));
            if (func_00175900(arg0, 1) != 0) {
                *(char *)(arg0 + 0x25C) = 0;
                func_0017C540(arg0, 1);
                return;
            }
            *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
            *(char *)(arg0 + 4) = 1;
            *(char *)(arg0 + 5) = 7;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0xD;
            return;
        }
        *(float *)(arg0 + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = *(float *)(*D_00275B40 + 8);
        func_00178B90(arg0, 1);
        fade = -0.2f;
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + fade;
        func_00179880((char *)arg0, (float *)(arg0 + 0x2EC));
        if (func_00175900(arg0, 1) != 0) {
            *(char *)(arg0 + 0x1F0) = 0x3E;
        } else {
            *(char *)(arg0 + 0x1F0) = 0xD;
        }
        return;
    }
    if (*(unsigned char *)(arg0 + 0x23A) == 0x5D) {
        func_0021D250((unsigned char *)arg0, 0);
    }
}
