// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Clean-store delay-slot-nop wall (tail-zeroing loop): target + mwcc 2.3.3 leave
// the bnez delay slot as a nop after the clean `sw zero` store; the pinned 991202
// build fills it (92.29%). Matched byte-exact on mwcc 2.3.3. The two free-list
// globals (D_00275BC4 head, D_00275BC8 count) are gp-relative, so -sdatathreshold
// must be >0 (8) to emit the single `%gp_rel(...)($gp)` access instead of lui/lo.
//
// Actor FREE (despawn back to pool). Takes the canonical pointer from +0x14
// (clearing the caller's copy), releases the bone slots (func_001AF800) and
// unlinks from the active doubly-linked list (func_001AFBC0), then pushes the
// actor onto the free singly-linked list (head D_00275BC4, next +0x1C, count
// D_00275BC8), clears the header words +0x00/04/08/0C, +0x36 h, +0x98 b, +0x90 w,
// and zeroes the 0x100-byte per-behavior scratch tail +0x1F0..+0x2F0.
extern void func_001AF800(unsigned char *actor);  // bone-slot release
extern void func_001AFBC0(unsigned char *actor);  // active-list unlink
extern unsigned char *D_00275BC4;  // free-list head
extern short D_00275BC8;           // free count

void func_001AFC10(unsigned char *handle) {
    unsigned char *self;
    int i;
    int *p;

    self = *(unsigned char **)(handle + 0x14);
    *(int *)(handle + 0x14) = 0;
    func_001AF800(self);
    func_001AFBC0(self);
    D_00275BC8 += 1;
    *(unsigned char **)(self + 0x1C) = D_00275BC4;
    D_00275BC4 = self;
    *(int *)(self + 0x0) = 0;
    *(int *)(self + 0x4) = 0;
    *(int *)(self + 0x8) = 0;
    *(int *)(self + 0xC) = 0;
    *(short *)(self + 0x36) = 0;
    self[0x98] = 0;
    *(int *)(self + 0x90) = 0;
    p = (int *)(self + 0x1F0);
    i = 0;
    do {
        i += 1;
        *p = 0;
        p += 1;
    } while (i < 0x40);
}
