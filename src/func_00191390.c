// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 96.97% NEARMISS).
// Fixes read from the original instructions:
// - The default group also lists move 0 (`case 0:` ahead of `case 1:`). mwcc drops
//   the move-0 test because it would branch straight to the next block, which in
//   turn keeps the move-1 test that the old C lost (same shape as the byte-matched
//   footstep switch in func_0016CD70: `case 0x0: case 0x1: ... default:`).
//   Behaviour is unchanged: move 0 already took the default path.

// Camera offsets for the player's current move (player +0x230). Clears
// cam+0x94 / +0x98 and sets the pair cam+0x8C / cam+0x5C:
// moves 2, 4, 0x0F: (-3, 1); 6..9, 0x2C, 0x2D: (0, 2); 0x13: (11, 2);
// anything else (0, 1 and 3 included): (2, 6) when cam+0x64 is -31.2 (bits 0xC1F99999),
// else (6, 2). When cam+0x6D is set, cam+0x98 becomes 23.
void func_00191390(char *cam, char *player) {
    *(float *)(cam + 0x94) = 0.0f;
    *(float *)(cam + 0x98) = 0.0f;
    switch (*(int *)(player + 0x230)) {
    case 0:
    case 1:
    case 3:
    default:
        if (*(float *)(cam + 0x64) == -31.1999989f) {
            *(float *)(cam + 0x8C) = 2.0f;
            *(float *)(cam + 0x5C) = 6.0f;
        } else {
            *(float *)(cam + 0x8C) = 6.0f;
            *(float *)(cam + 0x5C) = 2.0f;
        }
        break;
    case 2:
    case 4:
    case 0x0F:
        *(float *)(cam + 0x8C) = -3.0f;
        *(float *)(cam + 0x5C) = 1.0f;
        break;
    case 0x2C:
    case 0x2D:
    case 6:
    case 7:
    case 9:
    case 8:
        *(float *)(cam + 0x8C) = 0.0f;
        *(float *)(cam + 0x5C) = 2.0f;
        break;
    case 0x13:
        *(float *)(cam + 0x8C) = 11.0f;
        *(float *)(cam + 0x5C) = 2.0f;
        break;
    }
    if (cam[0x6D] != 0) {
        *(float *)(cam + 0x98) = 23.0f;
    }
}
