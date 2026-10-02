// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-frame driver of an object with a work block at self+0x1F0, gated by
// the scratchpad byte 0x70003B8D. Mode 1: unless self+0xD is 3, when
// func_001B2140(self) is nonzero the actor only runs its callback self+0x4C
// (when self+4 is nonzero) and returns. Modes 2 and 3: the actor runs only when
// self+0xD is 3 and self+4 is not 1. Otherwise self+0x52 = 0 and the state
// self+4 picks the handler (0: 0012E560, 1: 0012E840, 2: 00131650, 3: 00131E80,
// which ends the frame). After states 0..2 the four work timers (+0x66, +0x5C,
// +0x6C, +0x6A) count down to 0, self+0x54 = 0, func_001B5360(self) runs when
// work+0x6D is set, then func_001B0D80(self).
extern unsigned char D_70003B8D;
extern int func_001B2140(unsigned char *e);
extern void func_0012E560(unsigned char *self, unsigned char *sub);
extern void func_0012E840(unsigned char *self, unsigned char *sub);
extern void func_00131650(unsigned char *self, unsigned char *sub);
extern void func_00131E80(unsigned char *self, unsigned char *sub);
extern void func_001B5360(unsigned char *p);
extern void func_001B0D80(unsigned char *p);

void func_0012E3A0(unsigned char *self) {
    unsigned char *sub = self + 0x1F0;
    switch (D_70003B8D) {
    case 0:
        break;
    case 1:
        if (self[0xD] != 3 && func_001B2140(self) != 0) {
            if (self[4] != 0) {
                (*(void (**)(unsigned char *))(self + 0x4C))(self);
            }
            return;
        }
        break;
    case 2:
    case 3:
        if (self[0xD] != 3) {
            return;
        }
        if (self[4] == 1) {
            return;
        }
        break;
    case 4:
        break;
    }
    *(short *)(self + 0x52) = 0;
    switch (self[4]) {
    case 0:
        func_0012E560(self, sub);
        break;
    case 1:
        func_0012E840(self, sub);
        break;
    case 2:
        func_00131650(self, sub);
        break;
    case 3:
        func_00131E80(self, sub);
        return;
    }
    if (sub[0x66]) {
        sub[0x66]--;
    }
    if (*(unsigned short *)(sub + 0x5C)) {
        (*(unsigned short *)(sub + 0x5C))--;
    }
    if (sub[0x6C]) {
        sub[0x6C]--;
    }
    if (sub[0x6A]) {
        sub[0x6A]--;
    }
    *(short *)(self + 0x54) = 0;
    if (sub[0x6D]) {
        func_001B5360(self);
    }
    func_001B0D80(self);
}
