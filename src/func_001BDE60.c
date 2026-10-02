// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State handler (state byte self+4, sub-step self+5, wobble block
// self+0x1F0). State 0: func_001B0FD0(self), self+0 = 1, self+0x34 = the byte
// self+0x2E, self+0x2E (halfword) = 0, wobble+0x10 = 0. State 1: sub-step 0
// advances when self+0xB is set and runs func_001BBD20(self, 0); sub-step 1
// advances when bone_wobble_decay_1(wobble) returns nonzero (self+0xB = 0,
// func_001BBD20(self, 1)); sub-step 2 returns to 0 when bone_wobble_decay_0
// returns nonzero; then func_001C6380(self), self+1 = 1, func_001B1D20(self) and
// the callback self+0x4C. States 2 and 3: func_001AFC10(self).
extern int func_001B0FD0(unsigned char *self);
extern void func_001AFC10(void *self);
extern void func_001C6380(unsigned char *self);
extern void func_001B1D20(unsigned char *self);
extern void func_001BBD20(unsigned char *self, int step);
extern int bone_wobble_decay_0(unsigned char *w);
extern int bone_wobble_decay_1(unsigned char *w);

void func_001BDE60(unsigned char *self) {
    unsigned char *wob = self + 0x1F0;
    unsigned char st;
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        self[0] = 1;
        *(short *)(self + 0x34) = self[0x2E];
        *(short *)(self + 0x2E) = 0;
        *(int *)(wob + 0x10) = 0;
        break;
    case 1:
        st = self[5];
        switch (st) {
        case 0:
            if (self[0xB]) {
                self[5] = st + 1;
                func_001BBD20(self, 0);
            }
            break;
        case 1:
            if (bone_wobble_decay_1(wob)) {
                self[5]++;
                self[0xB] = 0;
                func_001BBD20(self, 1);
            }
            break;
        case 2:
            if (bone_wobble_decay_0(wob)) {
                self[5] = 0;
            }
            break;
        }
        func_001C6380(self);
        self[1] = 1;
        func_001B1D20(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
