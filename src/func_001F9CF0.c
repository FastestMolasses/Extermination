// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.94% NEARMISS).
// Fixes read from the original instructions:
//  - The old body was wrong in case 1's "not yet at the end" path: when the
//    whole ring (0x28 - 0x24 blocks) holds less than the remaining bytes, the
//    block count is the ring size, else the remaining bytes rounded up to
//    0x800-byte blocks (the arms were swapped).
//  - The state dispatch is a `switch` with case 0 falling into case 1, every
//    case ending in `break`; the "reached the end" path is the else arm of an
//    `!=` test.
//  - D_00810E90 (the frame counter) is a volatile unsigned global, re-read at
//    every use; the channel's active byte is read into an int local and
//    compared through a (signed char) cast (the target re-sign-extends it).
//  - Register order comes from the local declaration order (counter first),
//    the elapsed value declared before the start stamp, the second loop with
//    its own locals, and the remaining-bytes subtraction written before the
//    ring-size one.
//
// Per-frame streaming update over 3 channels (records of 0x60 bytes at
// D_00281FD0; channel active bytes at D_00282150 + 4 + i; per-channel ready
// flags D_008106F4 / D_008106F5). Active state 2 with no stop pending times the
// channel out against the frame counter (func_001FAAC0 on expiry). Record
// state 0 polls the stream position (func_0011A730) to pick the half buffer
// to refill and queues a read; state 1 advances the write pointer by the
// completed blocks, re-queues until the half is full, then flips halves and
// wraps the ring read position; a channel waiting to start (active 1) starts
// (func_0011A6A0) unless its ready flag is set; state 2 starts once the ready
// flag clears. After the three sub-updates, idle channels whose stream still
// runs are stopped (func_0011A6E8).
extern int func_0011A730(int a0);
extern int func_0011A6A0(unsigned long a0);
extern int func_0011A6E8(unsigned long a0);
extern void func_001FAAC0(int slot);
extern void func_001FA0D0(void);
extern void func_001FA5F0(void);
extern void func_001FA330(void);

extern char D_00281FD0[];
extern char D_00282150[];
extern unsigned char D_008106F4;
extern unsigned char D_008106F5;
extern volatile unsigned int D_00810E90;

void func_001F9CF0(void) {
    int i;
    unsigned char *flag;
    char *rec;
    char *ch;
    unsigned int el;
    unsigned int last;
    unsigned int end;
    unsigned int avail;
    unsigned int need;
    int st;

    rec = D_00281FD0;
    ch = D_00282150;
    i = 0;
    do {
        if (i != 0) {
            flag = &D_008106F5;
        } else {
            flag = &D_008106F4;
        }
        st = *(signed char *)(ch + 4);
        if (st != 0) {
            if ((signed char)st == 2 && *(int *)(rec + 0x20) == 0) {
                last = *(unsigned int *)(rec + 0x50);
                if (D_00810E90 < last) {
                    el = D_00810E90;
                    *(int *)(rec + 0x48) = *(int *)(rec + 0x4C) - ((-1 - last) + D_00810E90);
                } else {
                    el = D_00810E90 - last;
                    *(int *)(rec + 0x48) = *(int *)(rec + 0x4C) - el;
                }
                if (el >= *(unsigned int *)(rec + 0x4C)) {
                    func_001FAAC0(i);
                    goto next;
                }
            }
            switch (*(signed char *)(rec + 0)) {
            case 0:
                el = func_0011A730(*(int *)(rec + 4));
                if (el == 0) {
                    break;
                }
                if (el < *(unsigned int *)(rec + 0x18) >> 1) {
                    *(signed char *)(rec + 1) = 1;
                    *(unsigned int *)(rec + 0x38) = *(unsigned int *)(rec + 0x14) + (*(unsigned int *)(rec + 0x18) >> 1);
                } else {
                    *(signed char *)(rec + 1) = 2;
                    *(unsigned int *)(rec + 0x38) = *(unsigned int *)(rec + 0x14);
                }
                if (*(signed char *)(rec + 2) == *(signed char *)(rec + 1)) {
                    break;
                }
                *(signed char *)(rec + 0) = 1;
                *(signed char *)(rec + 3) = 1;
                avail = *(unsigned int *)(rec + 0x28) - *(unsigned int *)(rec + 0x30);
                need = ((*(unsigned int *)(rec + 0x18) >> 1) + 0x7FF) >> 11;
                if (need < avail) {
                    avail = need;
                }
                *(unsigned int *)(rec + 0x34) = avail;
            case 1:
                if (*(signed char *)(rec + 3) != 2) {
                    break;
                }
                *(unsigned int *)(rec + 0x38) += *(unsigned int *)(rec + 0x34) << 11;
                if (*(signed char *)(rec + 1) == 1) {
                    end = *(unsigned int *)(rec + 0x14) + *(unsigned int *)(rec + 0x18);
                } else {
                    end = *(unsigned int *)(rec + 0x14) + (*(unsigned int *)(rec + 0x18) >> 1);
                }
                if (*(unsigned int *)(rec + 0x38) != end) {
                    *(signed char *)(rec + 3) = 1;
                    *(unsigned int *)(rec + 0x30) = *(unsigned int *)(rec + 0x24);
                    need = end - *(unsigned int *)(rec + 0x38);
                    avail = *(unsigned int *)(rec + 0x28) - *(unsigned int *)(rec + 0x24);
                    if ((avail << 11) < need) {
                        *(unsigned int *)(rec + 0x34) = avail;
                    } else {
                        *(unsigned int *)(rec + 0x34) = (need + 0x7FF) >> 11;
                    }
                } else {
                    *(signed char *)(rec + 0) = 0;
                    *(signed char *)(rec + 3) = 0;
                    *(signed char *)(rec + 2) = *(signed char *)(rec + 1);
                    *(unsigned int *)(rec + 0x30) += *(unsigned int *)(rec + 0x34);
                    if (*(unsigned int *)(rec + 0x30) >= *(unsigned int *)(rec + 0x28)) {
                        *(unsigned int *)(rec + 0x30) = *(unsigned int *)(rec + 0x24) + (*(unsigned int *)(rec + 0x30) - *(unsigned int *)(rec + 0x28));
                    }
                    if (*(signed char *)(ch + 4) == 1) {
                        if (*flag != 0) {
                            *(signed char *)(rec + 0) = 2;
                            *flag = 1;
                        } else {
                            *(signed char *)(ch + 4) += 1;
                            *(unsigned int *)(rec + 0x50) = D_00810E90;
                            func_0011A6A0(*(unsigned long *)(rec + 8));
                        }
                    }
                }
                break;
            case 2:
                if (*flag == 0) {
                    *(signed char *)(rec + 0) = 0;
                    *(signed char *)(ch + 4) += 1;
                    *(unsigned int *)(rec + 0x50) = D_00810E90;
                    func_0011A6A0(*(unsigned long *)(rec + 8));
                }
                break;
            }
        }
    next:
        i += 1;
        ch += 1;
        rec += 0x60;
    } while (i < 3);

    func_001FA0D0();
    func_001FA5F0();
    func_001FA330();

    {
        int j;
        char *act;
        char *r;
        char *w;
        act = D_00282150;
        r = D_00281FD0;
        w = D_00282150;
        j = 0;
        do {
            if (*(signed char *)(act + 4) == 0 && func_0011A730(*(int *)(r + 4)) != 0) {
                func_0011A6E8(*(unsigned long *)(r + 8));
                *(int *)(w + 0x28) = 0;
                *(signed char *)(r + 3) = 0;
            }
            j += 1;
            act += 1;
            r += 0x60;
            w += 4;
        } while (j < 3);
    }
}
