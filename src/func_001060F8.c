// COMPILER: eegcc
// CFLAGS: -O2
// Movie library (SDK libmpeg): sets up the reference-picture pointers for
// one picture. In field mode 1 (structure == 1) a first field (no second
// field yet, top flag clear) allocates one buffer (func_00106948(1)) for
// both slots and the picture is set up once (func_00106278); otherwise each
// of the two slots gets its own buffer and set-up (the second 0x10 bytes
// further). The first set-up's pointer pair is copied to its second half.
// Every set-up call is func_00106278(p, a1, a2, a3, top, halve, 0): the 9th
// argument (top, stack slot 0) also gates the callee's writes to a1[], the
// 10th (halve, stack slot 1) makes the callee halve p[1] around its second
// decode step, and the 7th callee argument is always 0.
extern int func_00106948(int n);
extern void func_00106278(char *pic, int a1, int a2, int a3, int top, int halve, int t2);

void func_001060F8(char *pics, int a1, int *slots, int idx, int structure, int second,
                   int a2, int a3, int top, int halve) {
    char *p;
    int buf;

    if (structure == 1) {
        if (second == 0 && top == 0) {
            buf = func_00106948(1);
            slots[idx] = buf;
            slots[idx + 2] = buf;
        }
        p = pics + idx * 8;
        func_00106278(p, a1, a2, a3, top, halve, 0);
        *(int *)(p + 0x10) = *(int *)(p + 0);
        *(int *)(p + 0x14) = *(int *)(p + 4);
        return;
    }
    slots[idx] = func_00106948(1);
    func_00106278(pics + idx * 8, a1, a2, a3, top, halve, 0);
    slots[idx + 2] = func_00106948(1);
    func_00106278(pics + (idx * 8 + 0x10), a1, a2, a3, top, halve, 0);
}
