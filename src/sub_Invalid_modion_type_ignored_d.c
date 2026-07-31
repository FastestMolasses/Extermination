// NEARMISS sub_Invalid_modion_type_ignored_d  (vram 0xnvalid_modion_type_ignored_d, 0x334 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.94% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall, PLUS two splat symbol-resolution artifacts that make a true objdiff 100.0 unreachable for this function as currently disassembled. The instruction sequence is identical opcode-for-opcode across the whole function (div/mfhi/mflo, both 0x1000D400 poll loops, the branchless ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241408;
extern int D_00241428;
extern int D_002412EC;
extern int D_002414AC;
extern char D_002418F8[];
extern char D_0026B2E0[];
extern int *D_00241544;
extern int *D_00241548;
extern int *D_0024154C;

extern void func_0010A378(char *msg, int arg);
extern void sub_c_invalid_motion_type_d_2(int x, int y, int flags);

int sub_Invalid_modion_type_ignored_d(int mba, int field, int flags, int mtype)
{
    int x = mba % D_00241428;
    int y = mba / D_00241428;
    int px = x * 16;
    int py = y * 16;
    int first = flags & 1;
    char *p;
    int k;
    int *fb;

    if (first) {
        while ((*(volatile unsigned int *)0x1000D400 >> 8) & 1)
            ;
        p = D_002418F8;
        *(int *)(p + *(int *)(p + 0x280) * 0x140 + 0x138) = 0;
    } else {
        int i;
        int n;
        unsigned long *dst;

        if ((unsigned int)(mtype - 1) >= 3) {
            func_0010A378(D_0026B2E0, mtype);
            D_00241408 = 1;
            return 0;
        }
        sub_c_invalid_motion_type_d_2(px, py, flags);

        while ((*(volatile unsigned int *)0x1000D400 >> 8) & 1)
            ;

        p = D_002418F8;
        dst = (unsigned long *)((D_002412EC & 0x0FFFFFFF) | 0x20000010);
        n = *(int *)(p + *(int *)(p + 0x280) * 0x140 + 0x12C);
        for (i = 0; i < n; i++) {
            char *e;
            int id;
            p = D_002418F8;
            id = (i == n - 1) ? 0 : 3;
            e = p + (i * 4 + *(int *)(p + 0x280) * 0x140);
            dst[0] = ((unsigned long)(*(int *)(e + 8) & 0x0FFFFFFF) << 32) | 0x30000030;
            dst[2] = ((unsigned long)(*(int *)(e + 0x18) & 0x0FFFFFFF) << 32)
                   | ((unsigned long)id << 28) | 0x30;
            dst += 4;
        }
        __asm__("sync");
        p = D_002418F8;
        k = *(int *)(p + 0x280) * 0x140;
        *(volatile int *)0x1000D480 = *(int *)(p + k);
        *(volatile int *)0x1000D430 = D_002412EC;
        *(volatile int *)0x1000D420 = 0;
        *(volatile int *)0x1000D400 = 0x105;
        *(int *)(k + p + 0x138) = 1;
    }

    if (field == 1 && (flags & 2)) {
        p = D_002418F8;
        *(int *)(p + *(int *)(p + 0x280) * 0x140 + 0x134) = field;
    } else {
        p = D_002418F8;
        *(int *)(p + *(int *)(p + 0x280) * 0x140 + 0x134) = 0;
    }

    p = D_002418F8;
    *(int *)(p + *(int *)(p + 0x280) * 0x140 + 0x130) = first;

    if (D_002414AC == 3) {
        fb = D_00241544;
        p = D_002418F8;
        *(int *)(p + *(int *)(p + 0x280) * 0x140 + 0x128) =
            fb[0] + (x * fb[4] + y) * 0x180;
    } else {
        fb = (D_002414AC == 2) ? D_0024154C : D_00241548;
        p = D_002418F8;
        *(int *)(p + *(int *)(p + 0x280) * 0x140 + 0x128) =
            fb[0] + (x * fb[4] + y) * 0x180;
    }
    return 1;
}
