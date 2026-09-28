// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00823580 (splat/link name 00823540; overlay
// code is linked 0x40 below where it runs), 0x380 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Role: owner with state +4. 0: counter +0x200 = 0, +4 = 1. 1: at counter
//  0 and 10 calls func_001EFD20(0x80000041, D_700038A0) with a scratchpad
//  vector; at 60, 65, 70 and 75 calls func_001EFD90(0x80000074, D_700038A0,
//  D_700038B0) and keeps the four results at +0x1F0..+0x1FC (at 70, byte +4
//  of the first result is set to 3). The counter then counts up; past 80,
//  +4 = 3. 2, 3: func_001AFC10.
// Matching: the 0x700038B0 stores go through the extern (idiom-32).
extern void func_001AFC10(unsigned char *self);
extern char *func_001EFD20(int id, void *a);
extern int func_001EFD90(int id, void *a, void *b);
extern float D_700038A0[4];
extern float D_700038B0[4];

void overlay_AREA02_func_00823540(unsigned char *self) {
    int *blk = (int *)(self + 0x1F0);
    switch (self[4]) {
    case 0:
        blk[4] = 0;
        self[4] = 1;
    case 1:
        switch (blk[4]) {
        case 0:
            *(float *)0x700038A0 = 175.0f;
            *(float *)0x700038A4 = 10.0f;
            *(float *)0x700038A8 = 0.0f;
            *(float *)0x700038AC = 1.0f;
            func_001EFD20(0x80000041, D_700038A0);
            break;
        case 10:
            *(float *)0x700038A0 = 175.0f;
            *(float *)0x700038A4 = 20.0f;
            *(float *)0x700038A8 = 10.0f;
            *(float *)0x700038AC = 1.0f;
            func_001EFD20(0x80000041, D_700038A0);
            break;
        case 60:
            D_700038B0[0] = 0.0f;
            *(float *)0x700038A0 = 170.0f;
            *(float *)0x700038A4 = 20.0f;
            *(float *)0x700038A8 = 10.0f;
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038B4 = -1.5707964f;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038BC = 1.0f;
            blk[0] = func_001EFD90(0x80000074, D_700038A0, D_700038B0);
            break;
        case 65:
            D_700038B0[0] = 0.0f;
            *(float *)0x700038A0 = 140.0f;
            *(float *)0x700038A4 = 20.0f;
            *(float *)0x700038A8 = 10.0f;
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038B4 = -1.5707964f;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038BC = 1.0f;
            blk[1] = func_001EFD90(0x80000074, D_700038A0, D_700038B0);
            break;
        case 70:
            D_700038B0[0] = 0.0f;
            *(float *)0x700038A0 = 110.0f;
            *(float *)0x700038A4 = 20.0f;
            *(float *)0x700038A8 = 10.0f;
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038B4 = -1.5707964f;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038BC = 1.0f;
            blk[2] = func_001EFD90(0x80000074, D_700038A0, D_700038B0);
            if (blk[0] != 0) {
                ((unsigned char *)blk[0])[4] = 3;
            }
            break;
        case 75:
            D_700038B0[0] = 0.0f;
            *(float *)0x700038A0 = 80.0f;
            *(float *)0x700038A4 = 20.0f;
            *(float *)0x700038A8 = 10.0f;
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038B4 = -1.5707964f;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038BC = 1.0f;
            blk[3] = func_001EFD90(0x80000074, D_700038A0, D_700038B0);
            break;
        }
        blk[4]++;
        if (blk[4] > 0x50) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
