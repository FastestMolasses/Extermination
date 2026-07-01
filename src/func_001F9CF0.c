// NEARMISS func_001F9CF0  (vram 0x001F9CF0, 0x3DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.94% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-ORDER wall (residual). Same instruction COUNT as target (256=256), frame size matches exactly (0x50), and 120/256 instructions are byte-identical; almost all of the remaining diffs (110 ARG_MISMATCH out of 256) trace to ONE register-coloring swap: my record-base pointer (itera...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0011A730(int a0);
extern int func_0011A6A0(long long a0);
extern void func_0011A6E8(long long a0);
extern void func_001FAAC0(int slot);
extern void func_001FA0D0(void);
extern void func_001FA5F0(void);
extern void func_001FA330(void);

extern char D_00281FD0[];
extern char D_00282150[];
extern unsigned char D_008106F4;
extern unsigned char D_008106F5;
extern int D_00810E90;

void func_001F9CF0(void) {
    char *s2;
    char *s3;
    unsigned char *s1;
    int s0;
    int v0, v1;
    int a0, a1;

    s2 = D_00281FD0;
    s3 = D_00282150;
    s0 = 0;
    do {
        if (s0 != 0) {
            s1 = &D_008106F5;
        } else {
            s1 = &D_008106F4;
        }

        if (*(signed char *)(s3 + 4) == 0) {
            goto next;
        }

        if (*(signed char *)(s3 + 4) == 2 && *(int *)(s2 + 0x20) == 0) {
            int last = *(int *)(s2 + 0x50);
            if ((unsigned int)D_00810E90 < (unsigned int)last) {
                a1 = D_00810E90;
                *(int *)(s2 + 0x48) = *(int *)(s2 + 0x4C) - ((-1 - last) + D_00810E90);
            } else {
                a1 = D_00810E90 - last;
                *(int *)(s2 + 0x48) = *(int *)(s2 + 0x4C) - a1;
            }
            if ((unsigned int)a1 >= (unsigned int)*(int *)(s2 + 0x4C)) {
                func_001FAAC0(s0);
                goto next;
            }
        }

        v1 = *(signed char *)(s2 + 0);
        if (v1 == 2) {
            goto case2;
        } else if (v1 == 1) {
            goto case1;
        } else if (v1 != 0) {
            goto next;
        }

        /* case 0 */
        v0 = func_0011A730(*(int *)(s2 + 4));
        if (v0 == 0) {
            goto next;
        }
        if ((unsigned int)v0 < (unsigned int)((unsigned int)*(int *)(s2 + 0x18) >> 1)) {
            *(signed char *)(s2 + 1) = 1;
            v0 = *(int *)(s2 + 0x14) + ((unsigned int)*(int *)(s2 + 0x18) >> 1);
        } else {
            *(signed char *)(s2 + 1) = 2;
            v0 = *(int *)(s2 + 0x14);
        }
        *(int *)(s2 + 0x38) = v0;

        if (*(signed char *)(s2 + 2) == *(signed char *)(s2 + 1)) {
            goto next;
        }
        *(signed char *)(s2 + 0) = 1;
        *(signed char *)(s2 + 3) = 1;
        v1 = *(int *)(s2 + 0x28) - *(int *)(s2 + 0x30);
        a0 = (unsigned int)(((unsigned int)*(int *)(s2 + 0x18) >> 1) + 0x7FF) >> 0xB;
        if ((unsigned int)a0 < (unsigned int)v1) {
            v1 = a0;
        }
        *(int *)(s2 + 0x34) = v1;

    case1:
        if (*(signed char *)(s2 + 3) != 2) {
            goto next;
        }
        *(int *)(s2 + 0x38) = *(int *)(s2 + 0x38) + (*(int *)(s2 + 0x34) << 0xB);
        if (*(signed char *)(s2 + 1) == 1) {
            a1 = *(int *)(s2 + 0x14) + *(int *)(s2 + 0x18);
        } else {
            a1 = *(int *)(s2 + 0x14) + ((unsigned int)*(int *)(s2 + 0x18) >> 1);
        }
        if (*(int *)(s2 + 0x38) == a1) {
            goto reached;
        }
        *(signed char *)(s2 + 3) = 1;
        *(int *)(s2 + 0x30) = *(int *)(s2 + 0x24);
        v1 = *(int *)(s2 + 0x28) - *(int *)(s2 + 0x24);
        a0 = a1 - *(int *)(s2 + 0x38);
        if ((unsigned int)(v1 << 0xB) < (unsigned int)a0) {
            *(int *)(s2 + 0x34) = (unsigned int)(a0 + 0x7FF) >> 0xB;
        } else {
            *(int *)(s2 + 0x34) = v1;
        }
        goto next;

    reached:
        *(signed char *)(s2 + 0) = 0;
        *(signed char *)(s2 + 3) = 0;
        *(signed char *)(s2 + 2) = *(signed char *)(s2 + 1);
        *(int *)(s2 + 0x30) = *(int *)(s2 + 0x30) + *(int *)(s2 + 0x34);
        if ((unsigned int)*(int *)(s2 + 0x30) >= (unsigned int)*(int *)(s2 + 0x28)) {
            *(int *)(s2 + 0x30) = *(int *)(s2 + 0x24) + (*(int *)(s2 + 0x30) - *(int *)(s2 + 0x28));
        }

        if (*(signed char *)(s3 + 4) != 1) {
            goto next;
        }
        if (*s1 != 0) {
            *(signed char *)(s2 + 0) = 2;
            *s1 = 1;
            goto next;
        }
        v0 = *(signed char *)(s3 + 4) + 1;
        *(signed char *)(s3 + 4) = v0;
        *(int *)(s2 + 0x50) = D_00810E90;
        func_0011A6A0(*(long long *)(s2 + 8));
        goto next;

    case2:
        if (*s1 != 0) {
            goto next;
        }
        *(signed char *)(s2 + 0) = 0;
        v0 = *(signed char *)(s3 + 4) + 1;
        *(signed char *)(s3 + 4) = v0;
        *(int *)(s2 + 0x50) = D_00810E90;
        func_0011A6A0(*(long long *)(s2 + 8));

    next:
        s0 += 1;
        s3 += 1;
        s2 += 0x60;
    } while (s0 < 3);

    func_001FA0D0();
    func_001FA5F0();
    func_001FA330();

    s1 = (unsigned char *)D_00282150;
    s2 = D_00281FD0;
    s3 = D_00282150;
    s0 = 0;
    do {
        if (*(signed char *)(s1 + 4) == 0 && func_0011A730(*(int *)(s2 + 4)) != 0) {
            func_0011A6E8(*(long long *)(s2 + 8));
            *(int *)(s3 + 0x28) = 0;
            *(signed char *)(s2 + 3) = 0;
        }
        s0 += 1;
        s1 += 1;
        s2 += 0x60;
        s3 += 4;
    } while (s0 < 3);
}
