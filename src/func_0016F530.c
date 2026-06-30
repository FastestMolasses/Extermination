// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Entity (re)init / state-kick. On the object at arg0: sets the flag byte at
// +0x1F1 = 1; if arg1 == 0 also sets +0x318 = 3; if the pending byte at +0x317
// is set, clears it (the value written to +0x2F2/+0x2E is 1 either way). Then
// writes +0x2F2 = 1, the short at +0x2E = 1, and fires
// func_001FBD50(self, 0x162, 0, 300.0f) (0x43960000 == 300.0f).
// When the two globals say it is allowed (D_00810CA6 == 0 && D_00810D3C != 0)
// it fires a second func_001FBD50(&D_008102B0, 0x179, 0, 300.0f) and latches
// D_008106C7 = 1.
//
// func_001FBD50's signature is (char *self, int a, int b, float f) -- the
// trailing 300.0f is passed in $f12 and the first arg is the inherited $a0
// (self), which m2c misattributes. Matched with mwcc 2.3.3
// (mwcps2-2.3.3-000906); pinned 991202 reaches 90.0% (clean-store delay-slot
// nop the 991202 build fills and 2.3.3/target leave empty).
extern void func_001FBD50(char *p, int a, int b, float f);
extern char D_008102B0;
extern char D_008106C7;
extern unsigned char D_00810CA6;
extern unsigned char D_00810D3C;

void func_0016F530(char *arg0, int arg1) {
    *(char *)(arg0 + 0x1F1) = 1;
    if (arg1 == 0) {
        *(char *)(arg0 + 0x318) = 3;
    }
    if (*(unsigned char *)(arg0 + 0x317) != 0) {
        *(unsigned char *)(arg0 + 0x317) = 0;
    }
    *(char *)(arg0 + 0x2F2) = 1;
    *(short *)(arg0 + 0x2E) = 1;
    func_001FBD50(arg0, 0x162, 0, 300.0f);
    if (D_00810CA6 == 0 && D_00810D3C != 0) {
        func_001FBD50(&D_008102B0, 0x179, 0, 300.0f);
        D_008106C7 = 1;
    }
}
