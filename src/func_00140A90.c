// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame state machine driven by the phase byte at record+6 (values 0,1,2).
// Lowered as a 3-way switch (compares, no jump table); the original lays out
// case 0 first then case 1, with case 2 as an early return.
//   phase 0: advance to 1; func_001B1190(record+0x9A byte, 0); clear arg1+0x50
//            and +0x4C; if arg1+0x8C != 0 set arg1+0x81 = 0 and run
//            anim_clip_init(record, 0x25, 1.0f, 0.0f), else arg1+0x81 = 1 and
//            anim_clip_init(record, 0x23, 1.0f, 0.0f). Then
//            func_001FBD50(record, 0x811, 0, 300.0f). If arg1+0x89 != 0, clear
//            it, run func_0021C040(&D_008102B0, record), and if D_008106BD == 1
//            clear it.
//   phase 1: if arg1+0x30 has bit 0x1000: advance to 2; clear arg1+0x8E and
//            +0x81; func_001FBD50(record, 0x810, 0, 300.0f); if
//            func_001EFE00(0x8000001E, record) == 0 set record+4 = 3.
//   phase 2: nothing.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build caps at
// 81.9% (delay-slot / block-layout difference on the case bodies). The if/else
// chain must be written as a `switch` to reproduce the case-0-then-case-1
// physical block order. anim_clip_init / func_001FBD50 take their float
// constants in $f12 (1.0f / 300.0f). Verified objdiff 100% vs
// build/expected/func_00140A90.o.
extern void anim_clip_init(char *a, int b, float c, float d);
extern void func_001B1190(int a, int b);
extern void func_001FBD50(char *a, int b, int c, float d);
extern int func_001EFE00(int a, char *b);
extern void func_0021C040(void *a, char *b);
extern void *D_008102B0;
extern unsigned char D_008106BD;

void func_00140A90(char *arg0, char *arg1) {
    unsigned char m;

    m = *(unsigned char *)(arg0 + 6);
    switch (m) {
    case 0:
        *(unsigned char *)(arg0 + 6) = m + 1;
        func_001B1190(*(unsigned char *)(arg0 + 0x9A), m);
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        if (*(char *)(arg1 + 0x8C) != 0) {
            *(char *)(arg1 + 0x81) = 0;
            anim_clip_init(arg0, 0x25, 1.0f, 0.0f);
        } else {
            *(char *)(arg1 + 0x81) = 1;
            anim_clip_init(arg0, 0x23, 1.0f, 0.0f);
        }
        func_001FBD50(arg0, 0x811, 0, 300.0f);
        if (*(char *)(arg1 + 0x89) != 0) {
            *(char *)(arg1 + 0x89) = 0;
            func_0021C040(&D_008102B0, arg0);
            if (D_008106BD == 1) {
                D_008106BD = 0;
            }
        }
        break;
    case 1:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = m + 1;
            *(char *)(arg1 + 0x8E) = 0;
            *(char *)(arg1 + 0x81) = 0;
            func_001FBD50(arg0, 0x810, 0, 300.0f);
            if (func_001EFE00(0x8000001E, arg0) == 0) {
                *(char *)(arg0 + 4) = 3;
            }
        }
        break;
    case 2:
        break;
    }
}
