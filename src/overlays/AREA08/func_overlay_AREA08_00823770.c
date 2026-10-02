// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x008237B0 (splat/link name 00823770; overlay code
//  is linked 0x40 below where it runs), 0xBC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Covers the splat pieces 00823770, 008237B0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x823870 at spawn entry 3. +0x28 counts frames; every
//  20th frame func_001FBD50(self, 0x906, 0, 1000.0) when +0x2A equals 1 + 3 *
//  rand / 2^15 and D_0081076F is 0; +0x2A cycles 0..5.
#define S16(o) (*(short *)(self + (o)))
extern unsigned char D_0081076F;
extern int func_00122BB8(void);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);

void func_overlay_AREA08_00823770(unsigned char *self) {
    int x;
    S16(0x28)++;
    if (S16(0x28) % 20 == 0) {
        x = func_00122BB8() >> 16;
        x *= 3;
        x >>= 15;
        if (S16(0x2A) == x + 1 && D_0081076F == 0) {
            func_001FBD50(self, 0x906, 0, 1000.0f);
        }
        if (S16(0x2A) > 4) {
            S16(0x2A) = 0;
        } else {
            S16(0x2A) = S16(0x2A) + 1;
        }
    }
}
