// NEARMISS func_001FBDB0  (vram 0x001FBDB0, 0xC4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 98.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// One instruction: the original leaves a dead copy of voice into v0 at the shared exit (a
// branch-retarget leftover of a different exit layout); mwcc leaves the -1 load there.
// Result-variable, goto and nested forms measured lower.
//
// The function links from the asm body in src/func_001FBDB0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Positions a playing sound voice by its distance to the listener. voice is
// the sound handle (-1 = none: returns -1). Only when func_00119890(1, voice)
// reports 2 (entry voice of the 0x78-byte voice table at D_0027E0C0: +0x2E == 0
// and +0x32 == 1) does it ask func_001FBF50(actor, &vol, &pan, 0,
// range, falloff) for the distance attenuation; a non-zero result sets the
// voice with func_0011A218(voice, vol, pan) and returns voice. When the source
// is out of range the voice is stopped with func_0011A070(voice). Every other
// path returns -1.
extern int func_00119890(int port, int voice);
extern void func_0011A070(int voice);
extern void func_0011A218(int voice, int vol, int pan);
extern int func_001FBF50(char *actor, int *vol, int *pan, int mode, float range, float falloff);

int func_001FBDB0(char *actor, int voice, float range, float falloff) {
    int vol;
    int pan;

    if (voice == -1) {
        return -1;
    }
    switch (func_00119890(1, voice)) {
    case 2:
        if (func_001FBF50(actor, &vol, &pan, 0, range, falloff)) {
            func_0011A218(voice, vol, pan);
            return voice;
        }
        func_0011A070(voice);
        return -1;
    }
    return -1;
}
