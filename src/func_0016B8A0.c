// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0x20
//
// Per-state animation/turn driver dispatched on the state byte at arg0+6.
// State 0: bump state, kick clip 0x180 via func_001749A0(self,0x180,0,1.0f),
// zero arg0+0xC0. State 1: keyed off global level id D_00275B14 -- if 0x1E set
// state=0xA; if 0x34 bump state, compute a turn angle func_001B1470(D_00281B64 -
// arg0[0xC4]) (stashed to scratchpad 0x70003A20), then clamp the target facing at
// arg0+0x218 to D_00281B64 when |ang|<=pi/2 else func_001B1470(pi + D_00281B64),
// and copy the two globals D_00275B10 / D_00275B00[3] into arg0+0x2E0 / +0x2E8;
// all state-1 paths end by zeroing arg0+7. State 2: step arg0[0xC4] toward
// arg0[0x218] with func_001B12B0(goal,cur,0.1396f) and once it reaches the goal
// set state=0xA. State 10: bump state, kick clip 0x183 at 8.0f. State 11: once
// bit 0x8000 of arg0+0x200 clears, bump state and set global D_00275B00[2]=1.
// State 12: once bit 0x1000 of arg0+0x200 is set, pick recoil parms by the byte
// at arg0+0x23B (0x1E vs else -> arg0[5]/arg0[0x1F0] = 0x12,0x22 or 0x10,0x21),
// reset state, clear arg0+0x2F1, and kick func_001749A0(self,func_001885B0(self),
// 0, 16.0f).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 89.5%. Keys: (1) an ascending `switch` on the local state byte reproduces the
// target's descending compare chain with the case-0/case-10 branch-likely;
// (2) -sdatathreshold 0x20 puts D_00275B14/B10/B00 (<=0x20 bytes) in gp-relative
// small data while D_00281B64 (declared [16], 64 bytes) stays absolute; (3)
// D_00275B00 is declared as a struct so both its int field +8 and float field
// +0xC emit direct gp-relative accesses (an array + &elem cast forced an extra
// address add).
extern float func_0011DF78(float);
extern void func_001749A0(char *, int, int, float);
extern int func_001885B0(char *);
extern float func_001B12B0(float, float, float);
extern float func_001B1470(float);
extern int D_00275B14;
extern float D_00275B10;
struct S275B00 { int _0; int _4; int _8; float _c; };
extern struct S275B00 D_00275B00;
extern float D_00281B64[16];

void func_0016B8A0(char *arg0) {
    unsigned char st = *(unsigned char *)(arg0 + 6);
    float d;

    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0x180, 0, 1.0f);
        *(int *)(arg0 + 0xC0) = 0;
        break;
    case 1:
        if (D_00275B14 == 0x1E) {
            *(unsigned char *)(arg0 + 6) = 0xA;
        } else if (D_00275B14 == 0x34) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            d = func_001B1470(D_00281B64[0] - *(float *)(arg0 + 0xC4));
            *(float *)0x70003A20 = d;
            if (func_0011DF78(d) <= 1.5707964f) {
                *(float *)(arg0 + 0x218) = D_00281B64[0];
            } else {
                *(float *)(arg0 + 0x218) = func_001B1470(3.1415927f + D_00281B64[0]);
            }
            *(float *)(arg0 + 0x2E0) = D_00275B10;
            *(float *)(arg0 + 0x2E8) = D_00275B00._c;
        }
        *(char *)(arg0 + 7) = 0;
        break;
    case 2:
        d = func_001B12B0(*(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4), 0.13962634f);
        *(float *)(arg0 + 0xC4) = d;
        if (d == *(float *)(arg0 + 0x218)) {
            *(unsigned char *)(arg0 + 6) = 0xA;
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0x183, 0, 8.0f);
        break;
    case 11:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            D_00275B00._8 = 1;
        }
        break;
    case 12:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            if (*(unsigned char *)(arg0 + 0x23B) == 0x1E) {
                *(char *)(arg0 + 5) = 0x12;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x22;
            } else {
                *(char *)(arg0 + 5) = 0x10;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x21;
            }
            *(char *)(arg0 + 0x2F1) = 0;
            func_001749A0(arg0, func_001885B0(arg0), 0, 16.0f);
        }
        break;
    }
}
