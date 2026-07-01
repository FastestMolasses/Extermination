// NEARMISS func_00154120  (vram 0x00154120, 0x340 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two small residual clusters after full logic/structure recovery and the gp-rel array-over-declaration idiom (extern T SYM[N] with byte-size > sdatathreshold 4 to force absolute lui/lo addressing on D_008102B0/BF/D_00810350/58 while D_00275B40 stays gp-rel -- this alone took the match from 81.6% t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern short anim_advance_time(float t);
extern int func_0019AA80(void *a, void *b, int c);
extern int func_001028D0(void *a, void *b, void *c);
extern void func_00102760(void *a0, void *a1);
extern void func_00153ED0(void *a0, void *a1, int a2, float f12);
extern float func_001B1240(char *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001FBD50(int self, int a, int b, float c);
extern void func_001C68C0(char *p);
extern void func_001B17A0(char *p);

extern int D_00275B40;
extern unsigned char D_008102B0[16];
extern unsigned char D_008102BF[16];
extern int D_008104D4[2];
extern char D_00810320[];
extern float D_00810350[2];
extern float D_00810358[2];
extern char D_700038A0[];

void func_00154120(char *arg0, char *arg1) {
    unsigned char st;

    *(short *)(arg1 + 2) = anim_advance_time(1.0f);
    st = *(unsigned char *)(arg0 + 5);
    switch (st) {
    case 0:
        if (func_0019AA80(*(char **)((char *)D_00275B40 + 0x34) + 0xC0, *(char **)((char *)D_00275B40 + 0x40) + 0xC0, 0x20) != 0 && D_008102B0[0] == 1) {
            D_008102BF[0] = 2;
            *(int *)D_008104D4 = 0x40A00000;
            D_008102B0[0] |= 2;
            func_001028D0(D_700038A0, *(char **)((char *)D_00275B40 + 0x40) + 0xC0, *(char **)((char *)D_00275B40 + 0x34) + 0xC0);
            func_00102760(D_00810320, D_700038A0);
        }
        if (*(short *)(arg1 + 2) & 0x1000) {
            func_00153ED0(arg0, arg1, 0, 0.0f);
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            *(short *)(arg0 + 0x28) = 0x78;
        }
        break;
    case 1: {
        short t = *(short *)(arg0 + 0x28) - 1;
        *(short *)(arg0 + 0x28) = t;
        if (t == 0) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            func_00153ED0(arg0, arg1, 2, 0.0f);
        }
        *(float *)(arg0 + 0xC4) = func_001B12B0(func_001B1240(arg0 + 0xB0, D_00810350[0], D_00810358[0]), *(float *)(arg0 + 0xC4), 0.0698131695f);
        break;
    }
    case 2:
        if (*(short *)(arg1 + 2) & 0x1000) {
            func_00153ED0(arg0, arg1, 3, 0.0f);
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            *(float *)(arg0 + 0xC4) = func_001B1240(arg0 + 0xB0, D_00810350[0], D_00810358[0]);
            func_001FBD50((int)arg0, 0x431, 0, 300.0f);
        }
        break;
    case 3:
        if (func_0019AA80(*(char **)((char *)D_00275B40 + 0x34) + 0xC0, *(char **)((char *)D_00275B40 + 0x40) + 0xC0, 0x20) != 0) {
            *(char *)(arg0 + 4) = 2;
            *(unsigned char *)(arg0 + 5) = 0;
            if (D_008102B0[0] == 1) {
                D_008102BF[0] = 2;
                *(int *)D_008104D4 = 0x41700000;
                D_008102B0[0] |= 2;
                func_001028D0(D_700038A0, *(char **)((char *)D_00275B40 + 0x40) + 0xC0, *(char **)((char *)D_00275B40 + 0x34) + 0xC0);
                func_00102760(D_00810320, D_700038A0);
            }
        } else if (func_0019A570(*(char **)((char *)D_00275B40 + 0x34) + 0xC0, *(char **)((char *)D_00275B40 + 0x40) + 0xC0, 6, 0) != 0) {
            *(char *)(arg0 + 4) = 2;
            *(unsigned char *)(arg0 + 5) = 0;
        } else if (*(short *)(arg1 + 2) & 0x1000) {
            *(char *)(arg0 + 4) = 3;
            *(unsigned char *)(arg0 + 5) = 0;
        }
        break;
    }
    func_001C68C0(arg0);
    func_001B17A0(arg0);
    ((void (*)(char *))*(int *)(arg0 + 0x4C))(arg0);
}
