// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State-machine dispatch on the kind-0xE tendril field self[4]:
//   0 INIT (func_00154740), 1 ACTIVE tick (func_001549C0),
//   2 -> bump to state 3, 3 (func_001AFC10). Each handler gets
//   (self, self+0x1F0 = scratch/tint record).
// mwcc233 fills the case-2 beqz delay slot the same way CW did
// (the idiom-13 clean-store nop case the pinned 991202 build cannot match).
extern void func_00154740(unsigned char *self, unsigned char *scr);
extern void func_001549C0(unsigned char *self, unsigned char *scr);
extern void func_001AFC10(unsigned char *self, unsigned char *scr);

void func_001546C0(unsigned char *self) {
    unsigned char *scr = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_00154740(self, scr);
        break;
    case 1:
        func_001549C0(self, scr);
        break;
    case 2:
        self[4] = 3;
        break;
    case 3:
        func_001AFC10(self, scr);
        break;
    }
}
