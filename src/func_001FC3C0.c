// NEARMISS func_001FC3C0  (vram 0x001FC3C0, 0x154 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Only residual: the expected object renders scratchpad 0x70003B68,0x70003B8A as literal operands (not in build.py _SPAD_SYMS) while this C uses relocated externs; linked bytes identical. objdiff 100.0% with _SPAD_SYMS += 0x70003B68,0x70003B8A.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x70003B68, 0x70003B8A
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// objdiff is 100.0% once build.py _SPAD_SYMS lists these addresses (so the
// expected object carries the same relocations); the linked bytes are identical.

// Per-tick service for one sound slot: keeps *arg1 (the live voice handle) in step
// with the requested sound id arg2 -- drops the voice when the table says it died or
// the id changed, and (re)starts it on the 10-tick scratchpad cadence at 0x70003B68.

extern int D_70003B68;
extern short D_70003B8A;
extern int D_00281C30[];
extern int D_00281B70[];

extern void func_0011A070(int voice);
extern int func_001FBD50(int self, int id, int flags);
extern int func_001FBDB0(int self, int voice, int ang);

void func_001FC3C0(int arg0, int *arg1, int arg2) {
    int idx;
    int v1;
    int ang;
    int r;

    r = *arg1;
    idx = r;
    if (idx != -1) {
        v1 = D_00281C30[idx];
        if (v1 == -1) {
            *arg1 = -1;
            return;
        }
        if (arg2 != v1) {
            func_0011A070(idx);
            D_00281B70[*arg1] = -1;
            *arg1 = -1;
            return;
        }
        ang = D_70003B8A;
        if ((D_70003B68 + ang) % 10 == 0) {
            r = func_001FBDB0(arg0, idx, ang);
            *arg1 = r;
            if (r == -1) {
                D_00281B70[idx] = -1;
            }
        }
    } else {
        ang = D_70003B8A;
        if ((D_70003B68 + ang) % 10 == 0) {
            r = func_001FBD50(arg0, arg2, 0);
            *arg1 = r;
            if (r != -1) {
                D_00281B70[*arg1] = arg2;
            }
        }
    }
}
