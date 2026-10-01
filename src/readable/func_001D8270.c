// NEARMISS func_001D8270  (vram 0x001D8270, 0xC4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 95.92% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Boolean lowering of the final compare: the original reaches the shared exit through an extra
// branch and leaves a dead 1-load behind it; mwcc 2.3.3 / 2.4 emit the short form (if/else,
// result-variable and pointer-local spellings measured).
//
// The function links from the asm body in src/func_001D8270.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Actor-lighting fold gate (port ACTOR_LIGHTING.md: em_lighting_fold_gate).
// Returns 0 for the object types (byte +3) 3, 8, 9, 0x0B, 0x0D, 0x15, 0x16,
// 0x17, 0x3D and 0x3E; otherwise 1 when the float at (obj+0x44)->+0x20 is
// below 30.0, else 0.
int func_001D8270(unsigned char *obj) {
    switch (obj[3]) {
    case 0x03:
    case 0x08:
    case 0x09:
    case 0x0B:
    case 0x0D:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x3D:
    case 0x3E:
        return 0;
    }
    return *(float *)(*(char **)(obj + 0x44) + 0x20) < 30.0f;
}
