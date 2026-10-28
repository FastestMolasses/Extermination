// NEARMISS func_00203D30  (vram 0x00203D30, 0x128 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.19% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Remaining differences: only the saved-register choice for the two pointer parameters is
// swapped (target keeps arg1 in the first saved register and arg2, later reused for the
// copy result, in the second; mwcc picks the opposite). Every instruction, the 0x70 frame
// and all stack slots match. Unchanged by local-copy/declaration-order sweeps, struct
// typing, callee prototypes, control-flow/return spellings and a 520 s permuter run.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Fixes over the 78.08% version (read from the original instructions):
// - The function takes three parameters (the first unused); the old C had two, so
//   arg1/arg2 were in the wrong argument registers.
// - The region base is an int pointer with its size word read as arg2[0x14002]
//   (byte offset 0x50008), as in the byte-matched twin func_00203E60.
// - The destination capacity arg1[3] is one local, reused for the trailing
//   (cap - len) argument; the four out-values are separate locals declared a, c, b, d.
// - func_00206BA0 takes a fifth argument: the copy result r.
//
// SEMANTICS: ring-buffer / region copy clamp+dispatch. The run length is the bytes
// from the read position (arg1[2]) to the region end (arg2 + its size word),
// clamped to the capacity arg1[3]. func_00206B00 fills four region descriptors;
// func_00203F40 performs the copy with the two descriptor addresses mapped into
// the uncached 0x20000000 segment. On a positive result func_00206BA0 reports the
// two 64-bit fields at arg1+0x10/+0x18; if that fails func_00204080 is notified.
// func_00206B10 releases the region; returns 1 if any bytes moved.
extern char D_002DF740[];
extern char D_00273450[];
int func_00206B00();
int func_00203F40();
int func_00206BA0(void *a0, long long a1, long long a2, int a3, int t0);
int func_00206B10();
int func_00204080();

int func_00203D30(int unused, int *arg1, int *arg2) {
    int a, c, b, d;
    unsigned int start;
    unsigned int cap;
    unsigned int n;
    int r;

    start = arg1[2];
    cap = arg1[3];
    n = ((unsigned int)arg2 + arg2[0x14002]) - start;
    if (cap < n) {
        n = cap;
    }
    func_00206B00(D_002DF740, &a, &b, &c, &d);
    r = func_00203F40((a & 0xFFFFFFF) | 0x20000000, b,
                      (c & 0xFFFFFFF) | 0x20000000, d,
                      start, n, arg2, cap - n);
    if (r > 0) {
        if (func_00206BA0(D_002DF740, *(long long *)(arg1 + 4),
                          *(long long *)(arg1 + 6), a, r) == 0) {
            func_00204080(D_00273450);
        }
    }
    func_00206B10(D_002DF740, r);
    if (r > 0) {
        return 1;
    }
    return 0;
}
