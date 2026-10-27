// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.84% NEARMISS).
// Fixes read from the original instructions:
//  - case 1 of the outer switch ends in `break`, not `return`. With `return`
//    mwcc dropped the three unreachable copies of the 0x2005 upper-half load
//    that the original keeps after each inner case's branch (the slot-fill
//    leftovers); `break` restores them and the 556-byte size.

// Title-menu compositor. The task's byte +0xA controls one-time audio/
// highlight setup; byte +0xF selects the highlighted menu texture.
// The sprite primitive's seventh parameter is the full 64-bit TEX0.
extern void func_001ABF90(long long, long long, long long, long long);
extern int func_001FB9F0(int, int, int, int);
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, unsigned int, long long);

#define TITLE_TASK (*(unsigned char **)0x70003B6C)

void func_001AC7F0(void)
{
    long long new_game;
    long long load_game;
    long long options;
    unsigned char *task;
    unsigned char *sub;

    task = TITLE_TASK;
    sub = task + 0xA;
    switch (task[0xA]) {
    case 0:
        sub[0]++;
        *(short *)(TITLE_TASK + 0x18) = 0;
        TITLE_TASK[0x10] = 0;
        func_001FB9F0(1500, 4096, 4096, 4096);
        /* fall through */
    case 1:
        func_001ABF90(0x2005F00621322A00LL, 0x2005F08621322A40LL,
                       0x2005F20621322C00LL, 0x2005F28621322C40LL);
        /* Valid menu selectors are 0..2. The original has no default
         * assignments, so an invalid selector has no defined textures. */
        switch (TITLE_TASK[0xF]) {
        case 0:
            new_game = 0x2005F425E1422E40LL;
            load_game = 0x2005F405E1422E80LL;
            options = 0x2005F405E1422F00LL;
            break;
        case 1:
            new_game = 0x2005F405E1422E00LL;
            load_game = 0x2005F485E1422EC0LL;
            options = 0x2005F405E1422F00LL;
            break;
        case 2:
            new_game = 0x2005F405E1422E00LL;
            load_game = 0x2005F405E1422E80LL;
            options = 0x2005F4A5E1422F40LL;
            break;
        }
        func_00207D00(1, 0);
        func_00207E40(1, 0x77F0, 0x8120, 256, 128, 0x80808080U, new_game);
        func_00207E40(1, 0x77F0, 0x8230, 256, 128, 0x80808080U, load_game);
        func_00207E40(1, 0x77F0, 0x8320, 256, 128, 0x80808080U, options);
        break;
    }
}
