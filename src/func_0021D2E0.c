// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Per-state driver (state byte at arg0+7): state 0 arms the entity, evaluates its
// skeleton and seeds the 0x700038A0 scratch vec4 from the current bone block; state 1
// runs the countdown timer at arg0+0x28 and advances/notifies on underflow; state 2 is
// a no-op. When arg2==0 it always fades arg0+0xB4 by -0.2f and services arg0+0x2EC.
extern void anim_eval_skeleton(char *);
extern void func_001749A0(char *, int, int, float);
extern int func_00175900(char *, int);
extern void func_00179880(char *, char *);
extern void func_001AEDE0(int, int);
extern void func_001EFD90(int, void *, char *, char *);
extern char *D_00275B40;
extern char D_700038A0[];

void func_0021D2E0(char *arg0, short arg1, int arg2) {
    unsigned char st;
    short t;
    char *p;
    char *base;

    st = *(unsigned char *)(arg0 + 7);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 7) = st + 1;
        *(char *)(arg0 + 0) = 2;
        *(short *)(arg0 + 0x28) = arg1;
        *(int *)(arg0 + 0x220) = 0;
        if (*(unsigned char *)(arg0 + 0x25F) == 0 && !(*(short *)(arg0 + 0x300) & 0x8000)) {
            anim_eval_skeleton(arg0);
            base = D_00275B40;
            p = *(char **)(base + 4);
            *(float *)0x700038A0 = *(float *)(p + 0xC0);
            *(float *)0x700038A8 = *(float *)(*(char **)(base + 4) + 0xC8);
            *(float *)0x700038A4 = 0.1f + *(float *)(arg0 + 0x250);
            *(int *)0x700038AC = 0x3F800000;
            func_001EFD90(0x80000043, D_700038A0, arg0 + 0xB0, p);
        }
        break;
    case 1:
        if (*(unsigned char *)(arg0 + 0x1F0) == 0xE && *(unsigned char *)(arg0 + 0x319) != 0) {
            func_001749A0(arg0, 0x2B, 0, 1.0f);
        }
        t = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = t - 1;
        if (t == 0) {
            *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) + 1;
            if (*(unsigned char *)(arg0 + 0xF) != 0xB) {
                func_001AEDE0(4, 0);
            }
        }
        break;
    case 2:
        break;
    }
    if (arg2 == 0) {
        *(float *)(arg0 + 0xB4) += -0.2f;
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            *(int *)(arg0 + 0x2EC) = 0;
        }
    }
}
