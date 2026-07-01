// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_00103230(void *a, void *b, float v);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019A910(void *a, void *b, int mask);
extern int func_0019B7D0(void *a, void *b);

extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_70003910[];

void func_0018D330(char *arg0, char *arg1, int arg2, int arg3) {
    int st;

    st = 0;
    *(float *)0x700038A0 = *(float *)(arg1 + 0xB0);
    *(float *)0x700038A4 = *(float *)(arg1 + 0xB4) + 4.0f;
    *(float *)0x700038A8 = *(float *)(arg1 + 0xB8);
    *(int *)0x700038AC = 0x3F800000;
    *(float *)0x700038B0 = *(float *)(arg1 + 0xB0);
    *(float *)0x700038B4 = *(float *)(arg1 + 0xA4) - 2.0f;
    *(float *)0x700038B8 = *(float *)(arg1 + 0xB8);
    *(int *)0x700038BC = 0x3F800000;
    if (func_0019B7D0(D_700038A0, D_700038B0) != 0) {
        *(unsigned char *)(arg0 + 0x6D) = 1;
    } else {
        *(unsigned char *)(arg0 + 0x6D) = 0;
    }

    *(float *)0x700038A0 = *(float *)(arg1 + 0xB0);
    *(float *)0x700038A4 = *(float *)(arg1 + 0xB4) + 200.0f;
    *(float *)0x700038A8 = *(float *)(arg1 + 0xB8);
    *(int *)0x700038AC = 0x3F800000;
    if (func_0019A910(arg1 + 0xB0, D_700038A0, arg3) != 0) {
        if ((*(short *)(*(char **)0x700031D0 + 0x1A) & 0x8800) != 0) {
            st = 0x80;
            *(float *)(arg0 + 0x60) = *(float *)0x700031B4;
        }
    }

    if (arg2 == 2) {
        *(float *)0x700038A0 = *(float *)(arg0 + 0x10);
        *(float *)0x700038A4 = *(float *)(arg1 + 0xA4) + 11.0f;
        *(float *)0x700038A8 = *(float *)(arg0 + 0x18);
        *(int *)0x700038AC = 0x3F800000;
        *(float *)0x700038B0 = *(float *)(arg1 + 0xA0);
        *(float *)0x700038B4 = *(float *)(arg1 + 0xA4) + 11.0f;
        *(float *)0x700038B8 = *(float *)(arg1 + 0xA8);
        *(int *)0x700038BC = 0x3F800000;
        func_001028D0(D_70003910, D_700038A0, D_700038B0);
        func_00102760(D_70003910, D_70003910);
        func_00103230(D_70003910, D_70003910, 9.0f);
        func_001028B8(D_700038A0, D_70003910, D_700038B0);
        if (func_0019A910(D_700038B0, D_700038A0, arg3) != 0) {
            if ((*(short *)(*(char **)0x700031D0 + 0x1A) & 0x2000) != 0) {
                st |= 1;
            } else if ((*(short *)(*(char **)0x700031D0 + 0x1A) & 0x8800) != 0) {
                st |= 8;
            }
        }
        *(float *)0x700038B0 = *(float *)(arg1 + 0xA0);
        *(float *)0x700038B4 = *(float *)(arg1 + 0xA4) + 6.0f;
        *(float *)0x700038B8 = *(float *)(arg1 + 0xA8);
        *(int *)0x700038BC = 0x3F800000;
        func_001028B8(D_700038A0, D_70003910, D_700038B0);
        *(float *)0x700038A4 = *(float *)(arg1 + 0xA4) + 6.0f;
        if (func_0019A910(D_700038B0, D_700038A0, arg3) != 0) {
            if ((*(short *)(*(char **)0x700031D0 + 0x1A) & 0x2000) != 0) {
                st |= 0x10;
            }
        }
    } else {
        *(float *)0x700038A0 = *(float *)(arg0 + 0x10);
        *(float *)0x700038A4 = *(float *)(arg1 + 0xA4) + 11.0f;
        *(float *)0x700038A8 = *(float *)(arg0 + 0x18);
        *(int *)0x700038AC = 0x3F800000;
        *(float *)0x700038B0 = *(float *)(arg1 + 0xA0);
        *(float *)0x700038B4 = *(float *)(arg1 + 0xA4) + 11.0f;
        *(float *)0x700038B8 = *(float *)(arg1 + 0xA8);
        *(int *)0x700038BC = 0x3F800000;
        func_001028D0(D_700038C0, D_700038A0, D_700038B0);
        func_00102760(D_700038C0, D_700038C0);
        func_00103230(D_700038C0, D_700038C0, 20.0f);
        func_001028B8(D_700038A0, D_700038C0, D_700038B0);
        if (func_0019A910(D_700038B0, D_700038A0, arg3) != 0) {
            if ((*(short *)(*(char **)0x700031D0 + 0x1A) & 0x2000) != 0) {
                st |= 1;
            } else if ((*(short *)(*(char **)0x700031D0 + 0x1A) & 0x8800) != 0) {
                st |= 8;
            }
        }
    }

    *(short *)(arg0 + 0x5A) = st;
}
