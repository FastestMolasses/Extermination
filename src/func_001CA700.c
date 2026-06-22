// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Lazily creates/caches a sub-object at a->0x90 via func_001AF780; bails
// returning 0 if allocation fails. On success stores arg2 (short) at
// a->0x94, arg1 at obj->0x60, then hands obj+0x70 to func_001D0690 and
// returns 1.
extern int func_001AF780(void);
extern void func_001D0690(int);

int func_001CA700(int a, int arg1, short arg2) {
    int v0;
    v0 = *(int *)(a + 0x90);
    if (v0 == 0) {
        v0 = func_001AF780();
        *(int *)(a + 0x90) = v0;
        if (v0 == 0) {
            return 0;
        }
    }
    *(short *)(a + 0x94) = arg2;
    *(int *)(v0 + 0x60) = arg1;
    func_001D0690(v0 + 0x70);
    return 1;
}
