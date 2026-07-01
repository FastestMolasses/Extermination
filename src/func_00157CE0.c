// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Weapon/actor state transition on actor arg0 when its flags byte (arg0+0xB)
// has bit 4 set. Builds a rotation scratch at D_700038A0 via func_001B6F00(arg0,
// PI, &D_700038A0), then dispatches on the actor's type byte (arg0+3):
//  - type 0x38: if D_00810C7F==0 -> error 0x80000012 (func_001BA1A0/1F0),
//    set actor state byte 0 = 2, return 2; else if 2*(arg0+0x34 short) exceeds
//    D_00810CB2 -> error 0x80000024, state 2, return 2; else pick 0x41/0x42 for
//    D_00247274 by comparing (arg0+0xB4 float) < 6.0+D_00810354, then event
//    0x80000012 into D_002472B4 and return 1.
//  - other types: if (short)(D_00810C63*30)==D_00810CB4 -> error 0x80000022,
//    state 2, return 2; else if flags bit 0 set -> set arg0+0xA=1, state 2,
//    return 3; else event 0x8000000C into D_00247734 and return 1.
// Returns 0 when flag bit 4 is clear.
extern void func_001B6F00(void *, float, void *);
extern void func_001BA1A0(int, void *);
extern void func_001BA1F0(char *);
extern char D_700038A0;
extern char D_00246F20;
extern int D_00246FB4;
extern char D_002471E0;
extern int D_00247274;
extern int D_002472B4;
extern char D_002476A0;
extern int D_00247734;
extern float D_00810354;
extern unsigned char D_00810C63;
extern unsigned char D_00810C7F;
extern short D_00810CB2;
extern short D_00810CB4;

int func_00157CE0(char *arg0, int arg1) {
    int t;

    if (*(unsigned char *)(arg0 + 0xB) & 4) {
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x410CCCCD;
        *(int *)0x700038AC = 0x3F800000;
        func_001B6F00(arg0, 3.14159274f, &D_700038A0);
        if (*(unsigned char *)(arg0 + 3) == 0x38) {
            if (D_00810C7F == 0) {
                D_00246FB4 = 0x80000012;
                func_001BA1A0(arg1, &D_00246F20);
                func_001BA1F0(arg0);
                *(char *)arg0 = 2;
                return 2;
            }
            if (D_00810CB2 < *(short *)(arg0 + 0x34) * 2) {
                D_00246FB4 = 0x80000024;
                func_001BA1A0(arg1, &D_00246F20);
                func_001BA1F0(arg0);
                *(char *)arg0 = 2;
                return 2;
            }
            if (*(float *)(arg0 + 0xB4) < (6.0f + D_00810354)) {
                D_00247274 = 0x42;
            } else {
                D_00247274 = 0x41;
            }
            D_002472B4 = 0x80000012;
            func_001BA1A0(arg1, &D_002471E0);
            func_001BA1F0(arg0);
            return 1;
        }
        t = D_00810C63 * 30;
        if ((short)t == D_00810CB4) {
            D_00246FB4 = 0x80000022;
            func_001BA1A0(arg1, &D_00246F20);
            func_001BA1F0(arg0);
            *(char *)arg0 = 2;
            return 2;
        }
        if (*(unsigned char *)(arg0 + 0xB) & 1) {
            *(char *)(arg0 + 0xA) = 1;
            *(char *)arg0 = 2;
            return 3;
        }
        D_00247734 = 0x8000000C;
        func_001BA1A0(arg1, &D_002476A0);
        func_001BA1F0(arg0);
        return 1;
    }
    return 0;
}
