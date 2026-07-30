// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-state ambient behavior tick for actor `self` with anim/AI context `arg1`,
// dispatched on the state byte self[6].
// State 0 (enter): clear the two counters at arg1+0x50/+0x4C, then roll a random
//   clip code from the 2x16 table D_00245B20, row = (self[0x56] timer != 0), column
//   = (rand >> 7) & 0xF:
//     0 -> self[6] = 1, arm the arg1+0x28 countdown with (u16)(((rand>>18)&0x7F)+0x78)
//          and, if the flag word at self+0x2C (masked ~0x8000) is set, start clip 0
//     1 -> self[6] = 2 and start clip 1 (the `zi = 0; z = (float)zi;` staging is
//          load-bearing: it makes mwcc emit `mtc1 zero,$f13` ahead of `mtc1 $v0,$f12`)
//     2 -> hand off: self[5] = 1, self[6] = 0
// State 1: count the arg1+0x28 timer down; when it reaches 0 reset self[6].
// State 2: when arg1+0x20 has bit 0x1000 set, reset self[6].
// Tail (all states): re-read the self[0x56] timer. If nonzero, clear it once
// arg1+0x68 <= (float)timer or self[0xA]&1. If zero, run func_0014BB10 and, when
// arg1+0x38 != 0 or self[0xA]&1, hand off with self[5] = 1, self[6] = 0.
extern void anim_clip_init(int self, int clip, float a, float b);
extern int func_00122BB8(void);
extern void func_0014BB10(unsigned char *self, unsigned char *arg1);
extern unsigned char D_00245B20[];

void func_00147960(unsigned char *self, unsigned char *arg1) {
    short s;
    int r;
    int v;
    int b;
    int zi;
    float z;

    switch (self[6]) {
    case 0:
        s = *(short *)(self + 0x56);
        *(int *)(arg1 + 0x50) = 0;
        r = (s != 0) ? 1 : 0;
        *(int *)(arg1 + 0x4C) = 0;
        b = D_00245B20[(r << 4) + ((func_00122BB8() >> 7) & 0xF)];
        switch (b) {
        case 0:
            self[6] = 1;
            v = func_00122BB8();
            *(int *)(arg1 + 0x28) = (unsigned short)(((v >> 18) & 0x7F) + 0x78);
            if ((*(short *)(self + 0x2C) & 0xFFFF7FFF) != 0) {
                anim_clip_init((int)self, 0, 5.0f, 0.0f);
            }
            break;
        case 1:
            zi = 0;
            z = (float)zi;
            self[6] = 2;
            anim_clip_init((int)self, 1, 5.0f, z);
            break;
        case 2:
            self[5] = 1;
            self[6] = 0;
            break;
        }
        break;
    case 1:
        v = *(int *)(arg1 + 0x28);
        if (v != 0) {
            *(int *)(arg1 + 0x28) = v - 1;
        } else {
            self[6] = 0;
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            self[6] = 0;
        }
        break;
    }

    s = *(short *)(self + 0x56);
    if (s != 0) {
        if (*(float *)(arg1 + 0x68) <= (float)s || (self[0xA] & 1)) {
            *(short *)(self + 0x56) = 0;
        }
    } else {
        func_0014BB10(self, arg1);
        if (*(int *)(arg1 + 0x38) != 0 || (self[0xA] & 1)) {
            self[5] = 1;
            self[6] = 0;
        }
    }
}
