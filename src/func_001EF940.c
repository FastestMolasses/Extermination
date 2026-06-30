// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-entity update: if the global state D_008101E4 != 3 and the entity's
// handle at arg0+0x24 is valid (!= -1), build a transform scratch via
// func_00102948, then func_001FBF50(scratch, &outX, &outY, 0, x, y) using the
// entity's two floats at arg0+0x28/+0x2C; on success forwards the two outputs
// to func_001FB9F0(handle, 0x1000, outX, outY).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906) at 100.0; the pinned 991202
// build reaches 87.9% (the clean-store delay-slot-nop wall). The 0x300-byte
// frame is split into a 0xB0 scratch at sp+0x20 and a 0x248 scratch at sp+0xD0
// plus the two output ints at sp+0x318/0x31C.

extern void func_00102948(void *p);
extern int func_001FBF50(void *a, int *b, int *c, int d, float f12, float f13);
extern void func_001FB9F0(int a, int b, int c, int d);
extern unsigned char D_008101E4;

void func_001EF940(char *arg0) {
    int local[0xB0 / 4];
    int spD0[0x248 / 4];
    int out318;
    int out31C;

    if (D_008101E4 != 3 && *(int *)(arg0 + 0x24) != -1) {
        func_00102948(spD0);
        if (func_001FBF50(local, &out318, &out31C, 0,
                          *(float *)(arg0 + 0x28), *(float *)(arg0 + 0x2C)) != 0) {
            func_001FB9F0(*(int *)(arg0 + 0x24), 0x1000, out318, out31C);
        }
    }
}
