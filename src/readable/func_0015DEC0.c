// NEARMISS func_0015DEC0  (vram 0x0015DEC0, 0x4C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 82.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Exit layout: the original's 0x32 test branches with an empty delay slot to a shared return-0
// block in front of the return, while mwcc fills the slot and emits a second return-0 block
// (nested, switch, boolean and goto spellings measured).
//
// The function links from the asm body in src/func_0015DEC0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Is the current stage record (0x700031D0) +0x1A mode word a 0x20xx mode
// other than low bytes 0x46 and 0x32? Returns 1 if so, else 0.

int func_0015DEC0(void) {
    short m = *(short *)(*(char **)0x700031D0 + 0x1A);
    int lo = m & 0xFF;

    if (lo == 0x46) {
        return 0;
    }
    if (lo != 0x32 && (m & 0xFF00) == 0x2000) {
        return 1;
    } else {
        return 0;
    }
}
