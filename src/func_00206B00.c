// Tail-call thunk: forwards to func_00204390 with the pointer advanced past
// the object's 0x48-byte header. func_00204390 reads five arguments (the ring
// and four out-pointers it fills with two free-region address/size pairs, kept
// in s1..s4 across its WaitSema call), and the callers func_00203D30 /
// func_00206BF0 pass them here; the thunk forwards $a1..$t0 untouched. The old
// C took only the pointer (decomp2 lane round 1; same bytes).
extern void func_00204390(void *p, int *a1, int *a2, int *a3, int *a4);

void func_00206B00(char *p, int *a1, int *a2, int *a3, int *a4) {
    func_00204390(p + 0x48, a1, a2, a3, a4);
}
