// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// MATCH NOTE (m2-matching lane, 97.34% -> 100.0%): the state byte at +0x2F3
// is read into an INT and tested as `(unsigned char)st == 3 || ... == 4`. The
// casts give the target's `andi v1,v0,0xff` copy. The two separate compares
// keep mwcc from merging 3/4 into a range test, and the `||` lowering gives
// `beq 3 / bne 4` with the dead fall-through copy the target has. A `switch`
// lowers to `beq/beq/b`. The 0x1E..0x20 range test is `> 2` (idiom-28).
//
// Semantics: per-frame player-actor update (called from the gameplay frame
// func_001AE5E0 with D_00275B44). func_00102948 is a quadword copy (dst, src).
// Copy +0xA0 -> +0xB0, run func_0015BA50, set +0xBC = 1.0f. Then animate:
// state byte +0x2F3 == 0 with +0x303 clear evaluates the skeleton when the
// D_00248C90 entry for clip +0x20C is nonzero (else func_001C68C0); states 3/4
// run func_001C68C0; any other nonzero state runs func_001C6960. Then
// func_0015CF90, func_0015CBA0, func_00187350; +0xB4 below -200.0f (outside
// state 6 and state 2/0x16) forces state 6/0. A pending sound handle in +0x31B
// is stopped (func_0011A070) and cleared once the pose that owns event
// 0x5DD / 0x12E / 0x135 is no longer active. Finally +0xB0 -> +0xA0, +0xB0 is
// reloaded from the quadword at +0xC0 of the matrix in bone slot 1
// (*(D_00275B40 + 4)), and +0xB0 / +0xC0 are published to scratchpad
// D_70003B40 / D_70003B50.

extern void anim_eval_skeleton(char *);
extern void func_00102948(void *, void *);
extern void func_0011A070(int);
extern void func_0015BA50(char *);
extern void func_0015CBA0(char *);
extern void func_0015CF90(char *);
extern void func_00187350(char *);
extern void func_001C68C0(char *);
extern void func_001C6960(char *);
extern short D_00248C90[4];
extern char *D_00275B40;
extern char D_70003B40[8];
extern char D_70003B50[8];

void func_0015BCF0(char *arg0) {
    int st;
    unsigned char b31A;
    unsigned short evt;

    *(volatile int *)0x700031F0 = 0;
    func_00102948(arg0 + 0xB0, arg0 + 0xA0);
    func_0015BA50(arg0);
    *(int *)(arg0 + 0xBC) = 0x3F800000;

    st = *(unsigned char *)(arg0 + 0x2F3);
    if (st == 0) {
        if (*(unsigned char *)(arg0 + 0x303) == 0) {
            if (*(short *)((char *)D_00248C90 + *(short *)(arg0 + 0x20C) * 0xC) != 0) {
                anim_eval_skeleton(arg0);
            } else {
                func_001C68C0(arg0);
            }
        }
    } else if ((unsigned char)st == 3 || (unsigned char)st == 4) {
        func_001C68C0(arg0);
    } else {
        func_001C6960(arg0);
    }

    func_0015CF90(arg0);
    func_0015CBA0(arg0);
    func_00187350(arg0);

    if (*(unsigned char *)(arg0 + 4) != 6 &&
        (*(unsigned char *)(arg0 + 4) != 2 || *(unsigned char *)(arg0 + 5) != 0x16) &&
        *(float *)(arg0 + 0xB4) < -200.0f) {
        *(unsigned char *)(arg0 + 4) = 6;
        *(unsigned char *)(arg0 + 5) = 0;
    }

    b31A = *(unsigned char *)(arg0 + 0x31A);
    if (b31A != 0) {
        if (*(signed char *)(arg0 + 0x31B) != -1) {
            evt = *(unsigned short *)(arg0 + 0x31C);
            if (evt == 0x5DD) {
                if (*(unsigned char *)(arg0 + 0x275) != 4 || *(unsigned char *)(arg0 + 4) != 1 ||
                    (*(unsigned char *)(arg0 + 5) != 0x1D && (unsigned int)(*(unsigned char *)(arg0 + 5) - 0x1E) > 2)) {
                    func_0011A070(*(signed char *)(arg0 + 0x31B));
                    *(signed char *)(arg0 + 0x31B) = -1;
                    *(unsigned char *)(arg0 + 0x31A) = 0;
                }
            } else if (evt == 0x12E) {
                if (*(unsigned char *)(arg0 + 4) != 1 || *(unsigned char *)(arg0 + 5) != 0x1C) {
                    func_0011A070(*(signed char *)(arg0 + 0x31B));
                    *(signed char *)(arg0 + 0x31B) = -1;
                    *(unsigned char *)(arg0 + 0x31A) = 0;
                }
            } else if (evt == 0x135) {
                if (*(unsigned char *)(arg0 + 4) != 1 || *(unsigned char *)(arg0 + 5) != 0x17) {
                    func_0011A070(*(signed char *)(arg0 + 0x31B));
                    *(signed char *)(arg0 + 0x31B) = -1;
                    *(unsigned char *)(arg0 + 0x31A) = 0;
                }
            }
        }
    }

    func_00102948(arg0 + 0xA0, arg0 + 0xB0);
    func_00102948(arg0 + 0xB0, *(char **)(D_00275B40 + 4) + 0xC0);
    func_00102948(D_70003B40, arg0 + 0xB0);
    func_00102948(D_70003B50, arg0 + 0xC0);
}
