// NEARMISS func_0015BCF0  (vram 0x0015BCF0, 0x2A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.34% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc canonicalization of a 2-case dense switch (case 3/case 4 shared body): mwcc always tests value 4 before 3 with a fixed v0/v1 register coloring regardless of source case order or ||-form (tried switch 3-then-4, switch 4-then-3, if(3||4), if(4||3); switch scored best at 97.3%, others 95.7%). A...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

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
    unsigned char st;
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
    } else {
        switch (st) {
        case 3:
        case 4:
            func_001C68C0(arg0);
            break;
        default:
            func_001C6960(arg0);
            break;
        }
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
                    (*(unsigned char *)(arg0 + 5) != 0x1D && (unsigned int)(*(unsigned char *)(arg0 + 5) - 0x1E) >= 3)) {
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
