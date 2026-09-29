// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00824540 (splat/link name 00824500; overlay code is
// linked 0x40 below where it runs), 0x148 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Covers the splat pieces 00824500, 00824540 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x824540) copies the six-point area 0x829600 to the stack;
//  sub-state +5 0: when D_00810354 > 195 and func_001B1EA0(0, D_00810350,
//  area, 6) starts script 0x829340 (+5 = 1); 1 at script end D_00810809 = 1,
//  +0x2E = 0xFFFF, func_001C4760(0x1D, 1), +5 = 0. func_001B1B70 and +0x4C run
//  when D_00810809 == 0 or D_70003B92 != 0.
typedef struct { float v[6][4]; } Box __attribute__((aligned(16)));
extern Box D_overlay_AREA16_00829600;
extern unsigned char D_00810809;
extern unsigned char D_70003B92;
extern float D_00810350[];
extern char D_overlay_AREA16_00829340[];
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int a0, int a1);
extern void func_001B1B70(unsigned char *self);

void func_overlay_AREA16_00824500(unsigned char *self) {
    Box area = D_overlay_AREA16_00829600;
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (D_00810350[1] > 195.0f && func_001B1EA0(0, D_00810350, &area, 6) != 0) {
            func_001BA1A0(talk, D_overlay_AREA16_00829340);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            D_00810809 = 1;
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            func_001C4760(0x1D, 1);
            self[5] = 0;
        }
        break;
    }
    if (D_00810809 <= 0 || D_70003B92 != 0) {
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
    }
}
