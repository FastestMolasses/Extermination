// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00823AB0 (splat/link name 00823A70; overlay code
//  is linked 0x40 below where it runs), 0x8C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: group 0x82BA70 member. As 0x823A10 with 8.0.
extern int D_700038B0[4];
extern void func_001F4E20(void *a0, void *a1, float f12);

void func_overlay_AREA21_00823A70(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        *(volatile int *)0x700038B0 = 0x30;
        *(volatile int *)0x700038B4 = 0x80;
        *(volatile int *)0x700038B8 = 0x30;
        *(volatile int *)0x700038BC = 0x80;
        func_001F4E20(self + 0x100, D_700038B0, 8.0f);
        break;
    case 2:
        break;
    case 3:
        break;
    }
}
