// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x008239C0 (splat/link name 00823980; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: spawned by the 0x825950 manager (group 0x82A900); state 1 writes the
//  scratchpad colour 0x30/0x80/0x30/0x80 and calls func_001F4E20 with 5.0.
extern int D_700038B0[4];
extern void func_001F4E20(void *a0, void *a1, float f12);

void func_overlay_AREA01_00823980(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        *(volatile int *)0x700038B0 = 0x30;
        *(volatile int *)0x700038B4 = 0x80;
        *(volatile int *)0x700038B8 = 0x30;
        *(volatile int *)0x700038BC = 0x80;
        func_001F4E20(self + 0x100, D_700038B0, 5.0f);
        break;
    case 2:
        break;
    case 3:
        break;
    }
}
