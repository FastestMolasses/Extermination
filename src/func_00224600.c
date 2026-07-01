// NEARMISS func_00224600  (vram 0x00224600, 0x574 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.93% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring (4x c.eq.s operand-order swaps on reused ==0.0f/!=0.0f tests) + mwcc tail-merge (3-instr shared epilogue for the early return-0 path, 12 bytes short). Body/structure 100% correct; not crackable via source-level changes (tried operand-order flips, flattened conditions, ~10 att...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern int func_00122BB8(void);
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern int func_00175900(unsigned char *e, int f);
extern void func_00178B90(unsigned char *e, int f);
extern void func_00179880(unsigned char *e, void *p);
extern void func_00182870(unsigned char *e, int f);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern void func_0021C120(unsigned char *e);
extern int func_0021C190(unsigned char *e);
extern void func_0021C200(unsigned char *e);
extern void func_0021C270(unsigned char *e);
extern void func_0021C350(unsigned char *e);

extern int D_00275B00;
extern int **D_00275B40;
extern unsigned char D_008106F1[];

int func_00224600(unsigned char *arg0) {
    unsigned char st;
    int v0;

    st = *(unsigned char *)(arg0 + 7);
    switch (st) {
    case 0:
        if (*(float *)(arg0 + 0x224) == 0.0f) {
            if (*(float *)(arg0 + 0x22C) == 0.0f && !(*(unsigned char *)(arg0 + 0xF) & 2)) {
                return 0;
            }
        }
        func_001B61C0(0, 0xC0, 5, 1);
        if (*(float *)(arg0 + 0x224) != 0.0f) {
            func_001FBD50(arg0, 0x152, 0, 300.0f);
            func_0021C350(arg0);
        }
        if (*(float *)(arg0 + 0x22C) != 0.0f) {
            func_001FBD50(arg0, 0x153, 0, 300.0f);
            func_0021C270(arg0);
        }
        if (*(float *)(arg0 + 0x220) <= 0.0f) {
            if (*(unsigned char *)(arg0 + 0xF) == 0x63 ||
                *(unsigned char *)(arg0 + 0x234) == 1) {
                func_0021C200(arg0);
                v0 = 0x1E;
            } else {
                v0 = 0xA;
            }
        } else if (!(*(float *)(arg0 + 0x228) < 100.0f) && D_008106F1[0] != 0) {
            v0 = 0x14;
        } else {
            v0 = *(unsigned char *)(arg0 + 7) + 1;
        }
        *(unsigned char *)(arg0 + 7) = v0;
        if (*(unsigned char *)(arg0 + 0xF) & 2) {
            *(unsigned char *)(arg0 + 0xF) = 0;
        }
        break;
    case 1:
        *(unsigned char *)(arg0 + 7) = st + 1;
        if (*(unsigned char *)(arg0 + 0xF) & 2) {
            func_001749A0(arg0, 0x185, 0, 4.0f);
            *(unsigned char *)(arg0 + 0xF) = 0;
        } else if (func_00122BB8() & 1) {
            func_001749A0(arg0, 0x185, 0, 4.0f);
        } else {
            func_001749A0(arg0, 0x184, 0, 4.0f);
        }
        break;
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 7) = 0;
            func_001749A0(arg0, 0x180, 0, 8.0f);
            *(short *)(arg0 + 0x20E) = 0x3C;
        }
        break;
    case 0xA:
        *(unsigned char *)(arg0 + 7) = st + 1;
        func_001749A0(arg0, 0x187, 0, 8.0f);
        break;
    case 0xB:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(char *)(arg0 + 6) = 0x14;
            *(unsigned char *)(arg0 + 7) = 0;
            return 2;
        }
        break;
    case 0x14:
        *(unsigned char *)(arg0 + 7) = st + 1;
        func_001749A0(arg0, 0x184, 0, 4.0f);
        break;
    case 0x15:
        if (*(float *)(arg0 + 0x3C) <= 30.0f) {
            *(unsigned char *)(arg0 + 7) = st + 1;
            func_0021C120(arg0);
        }
        break;
    case 0x16:
        if (func_0021C190(arg0) != 0) {
            *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) + 1;
        }
        break;
    case 0x17:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 7) = 0;
            func_001749A0(arg0, 0x180, 0, 8.0f);
            *(short *)(arg0 + 0x20E) = 0x3C;
        }
        break;
    case 0x1E:
        *(unsigned char *)(arg0 + 7) = st + 1;
        func_001749A0(arg0, 0x186, 0, 8.0f);
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        *(char *)(arg0 + 0x25F) = 2;
        break;
    case 0x1F:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 7) = st + 1;
            (&D_00275B00)[2] = 1;
        }
        break;
    case 0x20:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 7) = st + 1;
        } else {
            *(float *)(arg0 + 0x38) = *(float *)((char *)*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)((char *)*D_00275B40 + 8);
            func_00178B90(arg0, 1);
            *(float *)(arg0 + 0x2EC) = *(float *)((char *)*D_00275B40 + 4) - *(float *)(arg0 + 0x2E4);
            *(float *)(arg0 + 0x2E4) = *(float *)((char *)*D_00275B40 + 4);
            if (*(float *)(arg0 + 0x2EC) < -4.0f) {
                *(float *)(arg0 + 0x2EC) = -4.0f;
            }
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2EC);
            func_00175900(arg0, 1);
        }
        break;
    case 0x21:
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            func_00182870(arg0, 1);
            func_001FBD50(arg0, 0x156, 0, 300.0f);
            *(char *)(arg0 + 4) = 2;
            *(char *)(arg0 + 5) = 3;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x3F;
            return 2;
        }
        break;
    }
    return 1;
}
