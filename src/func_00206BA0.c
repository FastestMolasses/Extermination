// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Builds a 0x20-byte argument struct on the stack from the incoming params
// (two long long, then int = a3 - a0->0x48, then the 5th int arg), then tail-
// calls func_00204D60(D_002DF788, &struct). mwcc 2.3.3 fills the jal delay slot
// with `sw t0,0x24(sp)` (matching the target); the pinned 991202 build instead
// hoists `addiu a0,v0,0` into the delay slot, so it does not match.
extern void func_00204D60(void *, void *);
extern char D_002DF788[];

typedef struct {
    long long a;
    long long b;
    int c;
    int d;
} Args;

void func_00206BA0(void *a0, long long a1, long long a2, int a3, int t0) {
    Args s;
    s.a = a1;
    s.b = a2;
    s.c = a3 - *(int *)((char *)a0 + 0x48);
    s.d = t0;
    func_00204D60(D_002DF788, &s);
}
