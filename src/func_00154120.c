// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.44% NEARMISS).
// Fixes read from the original instructions:
//  - anim_advance_time takes (self, dt) per its definition in src/; the file
//    declared it with only the float, which reordered the saved-register copies
//    of the two parameters around the call.
//  - D_008102B0 is one global block: the state byte (+0x00), mode byte (+0x0F),
//    vector (+0x70), target x/z (+0xA0/+0xA8) and a float timer (+0x224) are
//    fields of one symbol. Separate per-address externs let alias analysis keep
//    the state byte across the timer store and changed the timer's addressing.
//
// Per-frame state machine for a scripted actor: advances its animation, and in
// states 0-3 waits on proximity checks / animation flags, sets the shared
// block's mode and timer, turns toward the block's target point, then runs the
// common actor update and the actor's own per-frame callback.

extern short anim_advance_time(char *self, float dt);
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
typedef struct {
    unsigned char state;      /* 0x000 */
    char pad01[0xE];
    unsigned char mode;       /* 0x00F */
    char pad10[0x70 - 0x10];
    char vec[0x30];           /* 0x070 */
    float x;                  /* 0x0A0 */
    char padA4[4];
    float z;                  /* 0x0A8 */
    char padAC[0x224 - 0xAC];
    float timer;              /* 0x224 */
} Sys008102B0;
extern Sys008102B0 D_008102B0;
extern char D_700038A0[];

void func_00154120(char *arg0, char *arg1) {
    unsigned char st;

    *(short *)(arg1 + 2) = anim_advance_time(arg0, 1.0f);
    st = *(unsigned char *)(arg0 + 5);
    switch (st) {
    case 0:
        if (func_0019AA80(*(char **)((char *)D_00275B40 + 0x34) + 0xC0, *(char **)((char *)D_00275B40 + 0x40) + 0xC0, 0x20) != 0 && D_008102B0.state == 1) {
            D_008102B0.mode = 2;
            D_008102B0.timer = 5.0f;
            D_008102B0.state |= 2;
            func_001028D0(D_700038A0, *(char **)((char *)D_00275B40 + 0x40) + 0xC0, *(char **)((char *)D_00275B40 + 0x34) + 0xC0);
            func_00102760(D_008102B0.vec, D_700038A0);
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
        *(float *)(arg0 + 0xC4) = func_001B12B0(func_001B1240(arg0 + 0xB0, D_008102B0.x, D_008102B0.z), *(float *)(arg0 + 0xC4), 0.0698131695f);
        break;
    }
    case 2:
        if (*(short *)(arg1 + 2) & 0x1000) {
            func_00153ED0(arg0, arg1, 3, 0.0f);
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            *(float *)(arg0 + 0xC4) = func_001B1240(arg0 + 0xB0, D_008102B0.x, D_008102B0.z);
            func_001FBD50((int)arg0, 0x431, 0, 300.0f);
        }
        break;
    case 3:
        if (func_0019AA80(*(char **)((char *)D_00275B40 + 0x34) + 0xC0, *(char **)((char *)D_00275B40 + 0x40) + 0xC0, 0x20) != 0) {
            *(char *)(arg0 + 4) = 2;
            *(unsigned char *)(arg0 + 5) = 0;
            if (D_008102B0.state == 1) {
                D_008102B0.mode = 2;
                D_008102B0.timer = 15.0f;
                D_008102B0.state |= 2;
                func_001028D0(D_700038A0, *(char **)((char *)D_00275B40 + 0x40) + 0xC0, *(char **)((char *)D_00275B40 + 0x34) + 0xC0);
                func_00102760(D_008102B0.vec, D_700038A0);
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
