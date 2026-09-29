// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00823E40 (splat/link name 00823E00; overlay code is
// linked 0x40 below where it runs), 0x120 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00823E00, 00823E40 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x823E40) +0x2A counter: every 8th frame, once it passes 240
//  it re-arms +0x240 = 2 + a random 0..3 and clears +0x2A; while +0x240 > 0 it
//  counts +0x240 down, otherwise it rotates 300 units by the +0x1C object's
//  +0xC0 angles (func_00103230 into 0x700038A0), offsets that by its +0xB0
//  position, plays func_001FBD50(self, 0x164, 0, 800.0) and calls
//  func_001B0CD0(object, 0).
extern float D_700038A0[];
extern int func_00122BB8(void);
extern void func_00103230(void *a, void *b, float angle);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001B0CD0(void *a0, int a1);

void func_overlay_AREA15_00823E00(unsigned char *self) {
    unsigned char *obj = *(unsigned char **)(self + 0x1C);
    (*(short *)(self + 0x2A))++;
    if (*(short *)(self + 0x2A) % 8 == 0) {
        if (*(short *)(self + 0x2A) > 240) {
            self[0x240] = ((func_00122BB8() >> 16) * 4 >> 15) + 2;
            *(short *)(self + 0x2A) = 0;
        }
        if (self[0x240] > 0) {
            self[0x240]--;
        } else {
            func_00103230(D_700038A0, obj + 0xC0, 300.0f);
            D_700038A0[0] += *(float *)(obj + 0xB0);
            D_700038A0[1] += *(float *)(obj + 0xB4);
            D_700038A0[2] += *(float *)(obj + 0xB8);
            func_001FBD50(self, 0x164, 0, 800.0f);
            func_001B0CD0(obj, 0);
        }
    }
}
