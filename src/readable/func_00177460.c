// NEARMISS func_00177460  (vram 0x00177460, 0xA8 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 86.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Scheduling of the scratchpad vector: the original reads x back from 0x70003680 before storing z
// to 0x70003684 and reuses z from its register (a no-op float move), mwcc stores z first and
// reloads it (volatile, pointer and local spellings measured).
//
// The function links from the asm body in src/func_00177460.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Is the actor farther than its leash from its anchor? The x / z offsets of
// the anchor (+0x2E0 / +0x2E8) from the actor position (+0xB0 / +0xB8) go to
// the scratchpad (0x70003680 / 0x70003684) and the planar distance (sqrtf,
// func_0011E748) to 0x70003688. The leash is 5.0, or 8.0 when wide is set.
// The result (1 beyond the leash, else 0) is stored in +0x2F2 and returned.
extern float func_0011E748(float x);

unsigned char func_00177460(unsigned char *actor, int wide) {
    float leash;
    float dz;
    float d;
    float x;

    if (wide == 0) {
        leash = 5.0f;
    } else {
        leash = 8.0f;
    }
    *(float *)0x70003680 = *(float *)(actor + 0x2E0) - *(float *)(actor + 0xB0);
    *(float *)0x70003684 = *(float *)(actor + 0x2E8) - *(float *)(actor + 0xB8);
    d = func_0011E748(*(float *)0x70003680 * *(float *)0x70003680 + *(float *)0x70003684 * *(float *)0x70003684);
    *(volatile float *)0x70003688 = d;
    if (d > leash) {
        actor[0x2F2] = 1;
    } else {
        actor[0x2F2] = 0;
    }
    return actor[0x2F2];
}
