// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state entity driver, dispatched on the state byte at arg0+4 (a 4-way
// switch 0/1/2/3 with descending 3->2->1->0 dispatch in the target). State 0:
// looks up a 5-float (0x14-byte stride) row D_00248290[idx*5] for the short
// index at arg0+0x54, stashes the row pointer at arg0+0x30 and the callback
// func_0015AAF0 at arg0+0x34, zeroes the bytes at +0xC/+9, sets +0/+4 = 1,
// writes 2*row[0] / 2*row[2] to the uncached scratchpad floats at
// 0x700038A0/0x700038A8, then calls func_001E8E80(self, ushort@+0xE,
// &0x700038A0). State 1: func_001E9280() then func_001B17A0(self). States 2
// and 3 both call func_001AFC10(self) (shared tail).
//
// Matched 100.0 with mwcc 2.3.3 (mwccps2-2.3.3-000906); the pinned 991202
// build reaches 47.0%. Keys: (1) the function is void (no return value sets v0
// in any path); (2) cases 2 and 3 share one func_001AFC10(self) via a `case 2:
// case 3:` fallthrough, which reproduces the target's beql merge with the
// `paddub a0,s0` delay-slot fill; (3) the scratchpad stores use the literal
// absolute address 0x700038A0/0x700038A8, but the call argument uses the symbol
// &D_700038A0 (same numeric address, different relocation form in the target).
extern float D_00248290[];
extern void func_0015AAF0(void);
extern void func_001E8E80(char *self, unsigned short n, float *p);
extern void func_001E9280(void);
extern void func_001B17A0(char *p);
extern void func_001AFC10(char *p);
extern float D_700038A0;

void func_0015AB00(char *arg0) {
    char *self = arg0;
    short idx;
    float *row;

    switch (*(unsigned char *)(self + 4)) {
    case 0:
        idx = *(short *)(self + 0x54);
        row = &D_00248290[idx * 5];
        *(int *)(self + 0x30) = (int)row;
        *(int *)(self + 0x34) = (int)func_0015AAF0;
        *(char *)(self + 0xC) = 0;
        *(char *)(self + 9) = 0;
        *(char *)(self + 0) = 1;
        *(char *)(self + 4) = 1;
        *(float *)0x700038A0 = 2.0f * row[0];
        *(float *)0x700038A8 = 2.0f * row[2];
        func_001E8E80(self, *(unsigned short *)(self + 0xE), &D_700038A0);
        break;
    case 1:
        func_001E9280();
        func_001B17A0(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
