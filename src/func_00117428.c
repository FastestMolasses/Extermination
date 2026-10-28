// NEARMISS func_00117428  (vram 0x00117428, 0x2B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.16% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-27 (was 88.05%): the cursor block D_0027F740 is a word array
// (`D_0027F740[12]`), which makes ee-gcc keep the full symbol address in a register and
// use offset 0x30 like the original; the channel table is the struct voice array shared
// with func_00115850.
// Remaining differences: saved-register colouring of cand2 (original gives it the lowest
// priority, s4) and an explicit reload of -1 before the final conditional move; ee-gcc
// here substitutes cand2 (known -1) for the final -1, adding a reference to cand2. A tail
// ending in `return cand1;` (also -1 there) colours every register right (99.31%) but
// still not the explicit -1; the original tail spelling is not found yet.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// Sound-channel allocator. Scans the 48-entry channel table three times, round-robin from a
// shared cursor:
//   pass 1 (only when arg0 != 0): find a channel already playing this exact sound (state 2,
//                                 f20 == arg0, f22 == arg2) so it can be retriggered;
//   pass 2:                       find a completely free slot (f00 == 0, state != 3);
//   pass 3:                       score the busy slots and steal the cheapest one.
// arg1 is the caller's priority, compared against each entry's f1E.
//

struct voice {
    unsigned short f00, f02, f04, f06, f08, f0A, f0C, f0E;
    unsigned short f10, f12, f14, f16, f18, f1A, f1C, f1E;
    unsigned short f20, f22, f24, f26, f28, f2A, f2C, f2E;
    unsigned short f30, f32, f34, f36, f38, f3A, f3C, f3E;
    unsigned short f40, f42, f44, f46, f48, f4A, f4C, f4E;
    unsigned short f50, f52, f54, f56, f58, f5A, f5C, f5E;
    unsigned short f60, f62, f64, f66, f68;
};

extern struct voice D_0027CCC0[];
extern unsigned int D_0027F740[];

int func_00117428(int arg0, int arg1, int arg2)
{
    int i;
    int idx;
    int cand1 = -1;
    int cand2 = -1;
    int cand3 = -1;
    int best1 = -1;
    int best2 = -1;
    int best3 = -1;

    if (arg0 != 0) {
        for (i = 0; i < 0x30; i++) {
            idx = D_0027F740[12] % 0x30;
            if (D_0027CCC0[idx].f1A == 2 &&
                D_0027CCC0[idx].f20 == arg0 &&
                D_0027CCC0[idx].f22 == arg2)
                return idx;
            D_0027F740[12] = D_0027F740[12] + 1;
        }
    }

    for (i = 0; i < 0x30; i++) {
        idx = D_0027F740[12] % 0x30;
        if (D_0027CCC0[idx].f00 == 0 &&
            D_0027CCC0[idx].f1A != 3)
            return idx;
        D_0027F740[12] = D_0027F740[12] + 1;
    }

    for (i = 0; i < 0x30; i++) {
        idx = D_0027F740[12] % 0x30;
        if (D_0027CCC0[idx].f08 == 1) {
            if (D_0027CCC0[idx].f1A == 1)
                return idx;
            if (D_0027CCC0[idx].f1A == 2 &&
                arg1 >= D_0027CCC0[idx].f1E &&
                D_0027CCC0[idx].f0A < best2) {
                cand2 = idx;
                best2 = D_0027CCC0[idx].f0A;
            }
        } else if (D_0027CCC0[idx].f1A == 1) {
            if (D_0027CCC0[idx].f0A < best1) {
                cand1 = idx;
                best1 = D_0027CCC0[idx].f0A;
            }
        } else if (D_0027CCC0[idx].f1A == 2) {
            if (arg1 >= D_0027CCC0[idx].f1E &&
                D_0027CCC0[idx].f0A < best3) {
                cand3 = idx;
                best3 = D_0027CCC0[idx].f0A;
            }
        }
        D_0027F740[12] = D_0027F740[12] + 1;
    }

    if (cand1 != -1)
        return cand1;
    if (cand2 != -1)
        return cand2;
    if (cand3 != -1)
        return cand3;
    return -1;
}
