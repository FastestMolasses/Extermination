// NEARMISS func_00223C70  (vram 0x00223C70, 0x614 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered; genuine residual is the documented D_00810E70/0x70003B7x delay-slot wall (same class as src/func_0021D530.c): the target leaves a nop in the first beqz delay slot of the 'D_00810E70 & *0x70003B7E' / '... & *0x70003B7C' double-HW-register test, but BOTH mwcc builds ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern int func_00122BB8(void);
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern void func_0021C120(unsigned char *e);
extern int func_0021C190(unsigned char *e);
extern void func_0021D2E0(unsigned char *e, int a, int b);
extern void func_00178B90(unsigned char *e, int f);
extern int func_00175900(unsigned char *e, int f);
extern void func_00179880(unsigned char *e, void *p);
extern void func_00182870(unsigned char *e, int f);
extern void func_0021D250(unsigned char *e, int f);
extern int **D_00275B40;
extern int D_00275B00;
extern unsigned char D_008106F1[];
extern unsigned short D_00810E70[4];

void func_00223C70(unsigned char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        if (*(unsigned char *)(arg0 + 0x1F1) == 0) {
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50(arg0, 0x152, 0, 300.0f);
            if (*(float *)(arg0 + 0x220) <= 0.0f) {
                if (*(unsigned char *)(arg0 + 0xF) == 0x63 ||
                    *(unsigned char *)(arg0 + 0x234) == 1) {
                    *(unsigned char *)(arg0 + 6) = 0x1E;
                    break;
                }
                *(unsigned char *)(arg0 + 6) = 0xA;
                break;
            }
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            break;
        }
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(arg0, 0x153, 0, 300.0f);
        if (!(*(float *)(arg0 + 0x228) < 100.0f) && D_008106F1[0] != 0) {
            *(unsigned char *)(arg0 + 6) = 0x14;
            break;
        }
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        break;
    case 1:
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        if (*(unsigned char *)(arg0 + 0xF) & 2) {
            func_001749A0(arg0, 0x185, 0, 4.0f);
            *(unsigned char *)(arg0 + 0xF) = 0;
            break;
        }
        if (func_00122BB8() & 1) {
            func_001749A0(arg0, 0x185, 0, 4.0f);
            break;
        }
        func_001749A0(arg0, 0x184, 0, 4.0f);
        break;
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(char *)(arg0 + 4) = 1;
            if (D_00810E70[0] & *(unsigned short *)0x70003B7E) {
                *(char *)(arg0 + 5) = 0x20;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x35;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
                *(char *)(arg0 + 0x317) = 1;
                break;
            }
            if (D_00810E70[0] & *(unsigned short *)0x70003B7C) {
                *(char *)(arg0 + 5) = 0x1F;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x34;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
                *(char *)(arg0 + 0x317) = 1;
                break;
            }
            *(char *)(arg0 + 5) = 0x14;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x26;
            *(char *)(arg0 + 0x317) = 0;
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        func_001749A0(arg0, 0x187, 0, 8.0f);
        break;
    case 11:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            *(char *)(arg0 + 7) = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 12:
        func_0021D2E0(arg0, 0x78, 1);
        break;
    case 20:
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        func_001749A0(arg0, 0x184, 0, 4.0f);
        break;
    case 21:
        if (*(float *)(arg0 + 0x3C) <= 30.0f) {
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            func_0021C120(arg0);
        }
        break;
    case 22:
        if (func_0021C190(arg0) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        }
        break;
    case 23:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(char *)(arg0 + 4) = 1;
            if (D_00810E70[0] & *(unsigned short *)0x70003B7E) {
                *(char *)(arg0 + 5) = 0x20;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x35;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
                *(char *)(arg0 + 0x317) = 1;
                break;
            }
            if (D_00810E70[0] & *(unsigned short *)0x70003B7C) {
                *(char *)(arg0 + 5) = 0x1F;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x34;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
                *(char *)(arg0 + 0x317) = 1;
                break;
            }
            *(char *)(arg0 + 5) = 0x14;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x26;
            *(char *)(arg0 + 0x317) = 0;
        }
        break;
    case 30:
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        func_001749A0(arg0, 0x186, 0, 8.0f);
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        *(char *)(arg0 + 0x25F) = 2;
        break;
    case 31:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            (&D_00275B00)[2] = 1;
        }
        break;
    case 32:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            break;
        }
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
        break;
    case 33:
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            func_00182870(arg0, 1);
            func_001FBD50(arg0, 0x156, 0, 300.0f);
            *(char *)(arg0 + 4) = 2;
            *(char *)(arg0 + 5) = 3;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x3F;
            break;
        }
        if (*(unsigned char *)(arg0 + 0x23A) == 0x5D) {
            func_0021D250(arg0, 0);
        }
        break;
    }
}
