// NEARMISS func_001D04B0  (vram 0x001D04B0, 0x90 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 81.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Argument-copy scheduling: the original copies the second float parameter into f20 before the
// first call, saves a2 in that call's delay slot and moves obj into a1 in the second call's delay
// slot; mwcc 2.3.3 / 2.4 order the copies differently (declaration order, local copies and a
// staged zero measured).
//
// The function links from the asm body in src/func_001D04B0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Registers a drawn sprite for an object: the display handle comes from
// func_001CCF70(obj + 0x30); func_001CFA60 builds the 0x60-byte draw block
// from the object with the two float parameters, and func_001CFBE0(handle,
// a1, a2, block, 0) queues it.
extern int func_001CCF70(void *a0);
extern void func_001CFA60(void *block, void *obj, float a, float b);
extern void func_001CFBE0(int handle, int a1, void *a2, void *block, int a4);

void func_001D04B0(char *obj, int a1, void *a2, float a, float b) {
    char block[0x60];
    int handle = func_001CCF70(obj + 0x30);

    func_001CFA60(block, obj, a, b);
    func_001CFBE0(handle, a1, a2, block, 0);
}
