// COMPILER: eegcc
// CFLAGS: -O2

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 83.60% NEARMISS).
// Fixes read from the original instructions:
//  - The "forward branch-likely wall" was not there: the original has no annulled
//    branches here; the residual came from a wrong body shape.
//  - The two "no handle / probe failed" paths are one combined `||` condition
//    sharing one tail (size, flag 0x800, is-tty = 0).
//  - func_0010BF10 (tty probe) takes ONE argument, the s16 id at +0xE; the old
//    extern passed (id, ptr).
//  - Cursor stores are a chained assignment `+0x10 = +0 = ptr` (base first,
//    read cursor last); flag ORs precede the length stores as written below.
//
// Buffer setup for a stdio-style stream object (same shape as newlib's
// __smakebuf). If flag 0x2 (unbuffered) is set in the u16 at +0xC, point the
// base (+0x10) and read cursor (+0) at the inline byte at +0x43 with size 1.
// Otherwise, if the s16 id at +0xE is valid, stat it via func_00120578 on the
// reentrancy handle at +0x54; a regular-file mode (0x8000) whose seek hook at
// +0x28 is the default one sets flag 0x400 and blksize (+0x4C) = 0x400, any
// other outcome sets flag 0x800; a character-device mode (0x2000) marks the
// stream as a possible tty. Then allocate 0x400 bytes via func_00120F40: on
// failure fall back to the inline byte and flag 0x2; on success install the
// cleanup hook func_00120038 at handle+0x3C, set flag 0x80, wire base/cursor
// to the buffer with size 0x400, and if it may be a tty and func_0010BF10(id)
// says so, set flag 0x1 (line buffered).
extern int func_0010BF10(int fd);
extern int func_00120578(char *h, int fd, void *stat);
extern char *func_00120F40(char *h, int n);
extern int func_00120038;
extern int vtable_a0_at_0011FE90_off28;

void func_00120B98(char *fp) {
    int size;
    int couldbetty;
    char *p;
    int st[0x1c];

    if (*(unsigned short *)(fp + 0xC) & 2) {
        *(char **)(fp + 0x10) = *(char **)(fp + 0) = fp + 0x43;
        *(int *)(fp + 0x14) = 1;
        return;
    }
    if (*(short *)(fp + 0xE) < 0 ||
        func_00120578(*(char **)(fp + 0x54), *(short *)(fp + 0xE), st) < 0) {
        couldbetty = 0;
        size = 0x400;
        *(unsigned short *)(fp + 0xC) |= 0x800;
    } else {
        couldbetty = (st[1] & 0xF000) == 0x2000;
        size = 0x400;
        if ((st[1] & 0xF000) == 0x8000 &&
            *(int *)(fp + 0x28) == (int)&vtable_a0_at_0011FE90_off28) {
            *(unsigned short *)(fp + 0xC) |= 0x400;
            *(int *)(fp + 0x4C) = 0x400;
        } else {
            *(unsigned short *)(fp + 0xC) |= 0x800;
        }
    }
    if ((p = func_00120F40(*(char **)(fp + 0x54), size)) == 0) {
        *(unsigned short *)(fp + 0xC) |= 2;
        *(char **)(fp + 0x10) = *(char **)(fp + 0) = fp + 0x43;
        *(int *)(fp + 0x14) = 1;
    } else {
        *(int *)(*(char **)(fp + 0x54) + 0x3C) = (int)&func_00120038;
        *(unsigned short *)(fp + 0xC) |= 0x80;
        *(char **)(fp + 0x10) = *(char **)(fp + 0) = p;
        *(int *)(fp + 0x14) = size;
        if (couldbetty && func_0010BF10(*(short *)(fp + 0xE)))
            *(unsigned short *)(fp + 0xC) |= 1;
    }
}
