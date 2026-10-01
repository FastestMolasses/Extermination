// NEARMISS func_00191390  (vram 0x00191390, 0x108 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 96.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// The original keeps a compare-and-branch for move 1 that targets the same block as the default
// fall-through; mwcc removes the redundant test (two instructions).
//
// The function links from the asm body in src/func_00191390.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Camera offsets for the player's current move (player +0x230). Clears
// cam+0x94 / +0x98 and sets the pair cam+0x8C / cam+0x5C:
// moves 2, 4, 0x0F: (-3, 1); 6..9, 0x2C, 0x2D: (0, 2); 0x13: (11, 2);
// anything else (1 and 3 included): (2, 6) when cam+0x64 is -31.2 (bits 0xC1F99999), else
// (6, 2). When cam+0x6D is set, cam+0x98 becomes 23.
void func_00191390(char *cam, char *player) {
    *(float *)(cam + 0x94) = 0.0f;
    *(float *)(cam + 0x98) = 0.0f;
    switch (*(int *)(player + 0x230)) {
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
