// Tail-call thunk: forwards to func_00204490 with the pointer advanced past
// the object's 0x48-byte header. func_00204490 is (ring, count) and the
// callers pass (D_002DF740, r); the thunk forwards $a1 untouched. The old C
// took only the pointer (decomp2 lane round 1; same bytes).
extern void func_00204490(void *p, int n);

void func_00206B10(char *p, int n) {
    func_00204490(p + 0x48, n);
}
