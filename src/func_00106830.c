// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 60.94% NEARMISS).
// Fixes read from the original instructions:
//  - func_00106490 takes no arguments (its own definition in src/ is
//    `long func_00106490(void)`); its 64-bit return value, truncated to int,
//    is what lands in D_00241B98. The old C passed D_00241324 to it and stored
//    a sign-extended copy of the command word instead (wrong body).
//  - The poll loop is the sibling func_001066F8's while loop with the
//    post-increment compare (`count++ >= 0x1389`). The recorded
//    "register-hoist / frame-stride wall" was these two body errors.
//
// IPU command issue: busy-wait while the IPU is busy (IPU_CTRL 0x10002010
// busy bit set, no error), calling func_00109B70(D_002412F4) every 5001
// polls; write the command (arg | 0x40000000) to IPU_CMD (0x10002000),
// record the command's table entry D_00241170[cmd >> 28] in D_00241324,
// read the next 32-bit word via func_00106490 into D_00241B98 and reset the
// bit count D_00241B9C to 32.

extern int func_00109B70(int);
extern long func_00106490(void);
extern int D_002412F4;
extern int D_00241324;
extern int D_00241170[];
extern int D_00241B98;
extern int D_00241B9C;

void func_00106830(int arg)
{
    int count;
    unsigned int v;

    count = 0;
    while ((*(volatile unsigned int *)0x10002010 & 0x80004000) == 0x80000000) {
        if (count++ >= 0x1389) {
            func_00109B70(D_002412F4);
            count = 0;
        }
    }
    v = arg | 0x40000000;
    *(volatile unsigned int *)0x10002000 = v;
    D_00241324 = D_00241170[v >> 28];
    D_00241B98 = (int)func_00106490();
    D_00241B9C = 32;
}
