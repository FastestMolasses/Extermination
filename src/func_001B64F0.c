// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Entity-array sweep (stride 0x2C, terminated by *(s16*)p == -1): for entities
// whose flags pass (field4 & ~0xE0)==2 and whose state field (field6) is in
// {1,4,5,7}, and whose field8 has bit 0x40 set, clears bit (field2 & 0x1F) in
// a per-index 32-bit bit-table word at D_00810700[idx<<5] + (field2>>5)*4 + 0x160.
// Match keys: switch (not || chain) prevents mwcc fusing cases 4||5 into a range;
// ascending case order 1,4,5,7 reproduces the t3=7..t0=1 constant allocation;
// &D_00810700[idx<<5] array-index form gives global-address-first eval; the single
// inlined compound-assignment (no slot/mask temps) gives mask-before-address coloring.
// 991202 reaches only 87.7% (fills the bnel delay slots differently) -> 2.3.3 unlock.
extern unsigned char D_00810700[];

void func_001B64F0(char *p, int idx) {
    unsigned char *base;

    base = &D_00810700[idx << 5];
    while (*(short *)(p + 0) != -1) {
        if ((*(short *)(p + 4) & ~0xE0) == 2) {
            short st = *(short *)(p + 6);
            switch (st) {
            case 1:
            case 4:
            case 5:
            case 7:
                if (*(short *)(p + 8) & 0x40) {
                    unsigned char b = *(unsigned char *)(p + 2);
                    if (b != 0) {
                        *(int *)((char *)base + (((int)b >> 5) << 2) + 0x160) &= ~(1 << (b & 0x1F));
                    }
                }
                break;
            }
        }
        p += 0x2C;
    }
}
