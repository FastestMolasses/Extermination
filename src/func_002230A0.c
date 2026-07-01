// NEARMISS func_002230A0  (vram 0x002230A0, 0x650 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/every field/global access and all call-argument shapes are byte-identical. Sole residual: the arg0+0x2F1 sub-state->flag classifier ('flag=1; if(sub==0) flag=0; else if(sub!=1) flag=2;') triggers a dead re-materialization of the literal 1 and 2 at the merge point (idiom-13b) -- a 12-byte sch...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern void func_0021C350(unsigned char *e);
extern void func_0021C270(unsigned char *e);
extern unsigned char D_008106F1[];
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern int func_001885B0(unsigned char *e);
extern int D_00275B14;
extern void func_0021D2E0(unsigned char *e, int a, int b);
extern void func_00178B90(unsigned char *e, int f);
extern int func_00175900(unsigned char *e, int f);
extern void func_00179880(unsigned char *e, void *p);
extern void func_00182870(unsigned char *e, int f);
extern void func_0021D250(unsigned char *e, int f);
extern void func_0021D490(unsigned char *e);
extern void func_0021C120(unsigned char *e);
extern int func_0021C190(unsigned char *e);
extern int **D_00275B40;

void func_002230A0(unsigned char *arg0) {
    unsigned char st;
    int flag;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        func_001B61C0(0, 0xC0, 5, 1);
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        *(char *)(arg0 + 7) = 0;
        {
            unsigned char sub = *(unsigned char *)(arg0 + 0x2F1);
            flag = 1;
            if (sub == 0) {
                flag = 0;
            } else if (sub != 1) {
                flag = 2;
            }
        }
        if (*(unsigned char *)(arg0 + 0xF) & 2) {
            if (flag == 0) {
                flag = 1;
            }
            *(unsigned char *)(arg0 + 0xF) = 0;
        }
        if (*(float *)(arg0 + 0x224) != 0.0f) {
            func_001FBD50(arg0, 0x152, 0, 300.0f);
            func_0021C350(arg0);
        }
        if (*(float *)(arg0 + 0x22C) != 0.0f) {
            func_001FBD50(arg0, 0x153, 0, 300.0f);
            func_0021C270(arg0);
        }
        if (*(float *)(arg0 + 0x220) <= 0.0f) {
            *(unsigned char *)(arg0 + 6) = 0xA;
            break;
        }
        if (!(*(float *)(arg0 + 0x228) < 100.0f) && D_008106F1[0] != 0) {
            *(unsigned char *)(arg0 + 6) = 0x14;
            flag = 0;
        }
        if (flag == 0) {
            if (*(unsigned char *)(arg0 + 0x234) == 0) {
                func_001749A0(arg0, 0xDD, 0, 8.0f);
            } else {
                func_001749A0(arg0, 0x1C9, 0, 8.0f);
            }
        } else if (flag == 1) {
            func_001749A0(arg0, 0xDF, 0, 8.0f);
        } else {
            func_001749A0(arg0, 0xE0, 0, 8.0f);
        }
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x290);
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x298);
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            if (D_00275B14 == 0x1E) {
                *(char *)(arg0 + 5) = 0x12;
                *(unsigned char *)(arg0 + 6) = 0;
            } else if (D_00275B14 == 0x36) {
                *(char *)(arg0 + 5) = 0x12;
                *(unsigned char *)(arg0 + 6) = 0x28;
            } else {
                *(char *)(arg0 + 5) = 0x10;
                *(unsigned char *)(arg0 + 6) = 0;
            }
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(unsigned char *)(arg0 + 0x2F1) = 0;
            func_001749A0(arg0, func_001885B0(arg0), 0, 16.0f);
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0xDE, 0, 8.0f);
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        break;
    case 11:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
        }
        break;
    case 12:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            break;
        }
        *(float *)(arg0 + 0x38) = *(float *)((char *)*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = *(float *)((char *)*D_00275B40 + 8);
        func_00178B90(arg0, 1);
        *(float *)(arg0 + 0x2EC) = *(float *)((char *)*D_00275B40 + 4) - *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0x2E4) = *(float *)((char *)*D_00275B40 + 4);
        *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 13:
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            func_00182870(arg0, 1);
            func_001FBD50(arg0, 0x156, 0, 300.0f);
            if (*(unsigned char *)(arg0 + 0xF) == 0x63 ||
                *(unsigned char *)(arg0 + 0x234) == 1) {
                *(char *)(arg0 + 4) = 2;
                *(char *)(arg0 + 5) = 3;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x3F;
                break;
            }
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001749A0(arg0, 0x2A, 0, 1.0f);
            *(char *)(arg0 + 0x1F0) = 0x40;
            break;
        }
        if (*(unsigned char *)(arg0 + 0x23A) == 0x5D) {
            func_0021D250(arg0, 0);
        }
        break;
    case 14:
        if (*(float *)(arg0 + 0x3C) <= 20.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_0021D490(arg0);
        }
        break;
    case 15:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(char *)(arg0 + 7) = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 16:
        func_0021D2E0(arg0, 0x78, 0);
        break;
    case 20:
        if (*(float *)(arg0 + 0x3C) <= 8.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_0021C120(arg0);
        }
        break;
    case 21:
        if (func_0021C190(arg0) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            break;
        }
        *(int *)(arg0 + 0x204) = 0x3DCCCCCD;
        break;
    case 22:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
            if (D_00275B14 == 0x1E) {
                *(char *)(arg0 + 5) = 0x12;
            } else {
                *(char *)(arg0 + 5) = 0x10;
            }
            *(short *)(arg0 + 0x20E) = 0x3C;
            func_001749A0(arg0, func_001885B0(arg0), 0, 16.0f);
            break;
        }
        *(int *)(arg0 + 0x204) = 0x3E800000;
        break;
    }
}
