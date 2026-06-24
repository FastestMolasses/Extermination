// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Anim-state stepper dispatched on the state byte arg0[6]. arg0 = entity ($a0),
// arg1 = secondary anim record ($a1).
//  case 0: advance state, arg1[0x75]=0, arg1[0x40]=1.0f, arg1[0x48]=1.5f,
//          arg1[0x4C]=-1.0f, arg1[0x50]=0; play clip 0x1D (speed 0.0f); fire
//          event 0x838; then arg0[0xC4] = func_001B1470(PI + func_0011E620(
//          arg0[0x70], arg0[0x78])). Then shared hit-flag tail (arg1[0x7B]).
//  case 1: if arg1[0x74] has bit 2, clear arg1[0x4C]. If arg1[0x30] has bit
//          0x1000: advance state, arg0[0xC4]=func_001B1470(PI + arg0[0xC4]),
//          play clip 0x1E (1.0f); if entity short arg0[0x34]==0 then arg0[5]=2,
//          arg0[6]=0, arg1[0x7C]=1.
//  case 2: if arg1[0x30] has bit 0x1000, reset actor record (arg0[4]=1,[5]=2,
//          [6]=0,[0]=1; arg0[0x36]=0; arg1[0x72]=0x5A, arg1[0x64]=300,
//          arg1[0x62]=((func_00122BB8()>>15)&0xFF)+300).
// PI constant is 0x40490FDB (3.14159265f). arg1[0x40/48/4C] init as raw float
// bit patterns. D_008102B0/D_008106BD stay hi/lo.
//
// mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build caps at 85.67% (it
// fills clean-store / float-compare delay slots differently). Verified objdiff 100%.
extern void anim_clip_init(char *a, int b, float c, float d);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_0021C040(void *a, char *b);
extern float func_0011E620(float a, float b);
extern float func_001B1470(float a);
extern int func_00122BB8(void);
extern int D_008102B0[16];
extern unsigned char D_008106BD[16];

void func_001453C0(char *arg0, char *arg1) {
    unsigned char v = *(unsigned char *)(arg0 + 6);
    int r;

    switch (v) {
    case 0:
        *(unsigned char *)(arg0 + 6) = v + 1;
        *(char *)(arg1 + 0x75) = 0;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(int *)(arg1 + 0x48) = 0x3FC00000;
        *(int *)(arg1 + 0x4C) = 0xBF800000;
        *(int *)(arg1 + 0x50) = 0;
        anim_clip_init(arg0, 0x1D, 0.0f, 0.0f);
        func_001FBD50(arg0, 0x838, 0, 300.0f);
        *(float *)(arg0 + 0xC4) =
            func_001B1470(3.14159265f + func_0011E620(*(float *)(arg0 + 0x70),
                                                      *(float *)(arg0 + 0x78)));
        if (*(char *)(arg1 + 0x7B) != 0) {
            *(char *)(arg1 + 0x7B) = 0;
            func_0021C040(D_008102B0, arg0);
            if (D_008106BD[0] == 1) {
                D_008106BD[0] = 0;
            }
        }
        break;
    case 1:
        if (*(char *)(arg1 + 0x74) & 2) {
            *(int *)(arg1 + 0x4C) = 0;
        }
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(float *)(arg0 + 0xC4) =
                func_001B1470(3.14159265f + *(float *)(arg0 + 0xC4));
            anim_clip_init(arg0, 0x1E, 1.0f, 0.0f);
            if (*(short *)(arg0 + 0x34) == 0) {
                *(char *)(arg0 + 5) = 2;
                *(char *)(arg0 + 6) = 0;
                *(char *)(arg1 + 0x7C) = 1;
            }
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            *(char *)(arg0 + 5) = 2;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg1 + 0x72) = 0x5A;
            *(short *)(arg1 + 0x64) = 0x12C;
            r = func_00122BB8();
            *(short *)(arg1 + 0x62) = ((r >> 15) & 0xFF) + 0x12C;
        }
        break;
    }
}
