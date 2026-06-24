// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Anim-state stepper, dispatched on the state byte arg0[6]. arg0 = entity ($a0),
// arg1 = secondary anim record ($a1).
//  case 0: advance state, set arg1[0x40]=1.0f, clear arg1[0x50]/[0x4C]. Read the
//          entity flags short arg0[0x36]: if bit 0x8000 set, play clip 0x1F and
//          fire event 0x839; else set arg1[0x72]=10 if bit 0x1000 else 0, set
//          arg0[0]=1, clear arg0[0x36], then pick clip from gp-rel short table
//          D_002753E8 indexed by (func_00122BB8()>>17)%3 and fire event 0x838.
//          Both anim_clip_init at speed 1.0f. Then the shared hit-flag tail: if
//          arg1[0x7B] set, clear it, run func_0021C040(&D_008102B0, arg0), and if
//          global D_008106BD==1 clear it.
//  case 1: if arg1[0x30] has bit 0x1000, reset the actor record (arg0[4]=1,
//          [5]=2, [6]=0, [0]=1; arg0[0x36]=0; arg1[0x64]=300, arg1[0x72]=0x5A,
//          arg1[0x62]=((func_00122BB8()>>15)&0xFF)+300).
// D_002753E8 is gp-rel -> -sdatathreshold 4. D_008102B0/D_008106BD stay hi/lo.
//
// mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build caps at 83.92% (it
// fills clean-store / branch delay slots differently). Verified objdiff 100%.
extern void anim_clip_init(char *a, int b, float c, float d);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_0021C040(void *a, char *b);
extern int func_00122BB8(void);
extern short D_002753E8[1];
extern int D_008102B0[16];
extern unsigned char D_008106BD[16];

void func_00145210(char *arg0, char *arg1) {
    unsigned char v = *(unsigned char *)(arg0 + 6);
    short s;
    int r;

    switch (v) {
    case 0:
        *(unsigned char *)(arg0 + 6) = v + 1;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        s = *(short *)(arg0 + 0x36);
        if (s & 0x8000) {
            anim_clip_init(arg0, 0x1F, 1.0f, 0.0f);
            func_001FBD50(arg0, 0x839, 0, 300.0f);
        } else {
            if (s & 0x1000) {
                *(char *)(arg1 + 0x72) = 0xA;
            } else {
                *(char *)(arg1 + 0x72) = 0;
            }
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg0 + 0x36) = 0;
            r = func_00122BB8();
            anim_clip_init(arg0, D_002753E8[(r >> 17) % 3], 1.0f, 0.0f);
            func_001FBD50(arg0, 0x838, 0, 300.0f);
        }
        if (*(char *)(arg1 + 0x7B) != 0) {
            *(char *)(arg1 + 0x7B) = 0;
            func_0021C040(D_008102B0, arg0);
            if (D_008106BD[0] == 1) {
                D_008106BD[0] = 0;
            }
        }
        break;
    case 1:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            *(char *)(arg0 + 5) = 2;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg1 + 0x64) = 0x12C;
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg1 + 0x72) = 0x5A;
            r = func_00122BB8();
            *(short *)(arg1 + 0x62) = ((r >> 15) & 0xFF) + 0x12C;
        }
        break;
    }
}
