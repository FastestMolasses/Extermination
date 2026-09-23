// NEARMISS func_001A8BE0  (vram 0x001A8BE0, 0xF4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Only residual: the expected object renders scratchpad 0x70003B86 as literal operands (not in build.py _SPAD_SYMS) while this C uses relocated externs; linked bytes identical. objdiff 100.0% with _SPAD_SYMS += 0x70003B86.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x70003B86
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// objdiff is 100.0% once build.py _SPAD_SYMS lists these addresses (so the
// expected object carries the same relocations); the linked bytes are identical.

extern void func_001A8660(int, unsigned char *);
extern void func_001A8840(int, unsigned char *);
extern void func_001A8970(int, unsigned char *);
extern unsigned char **D_00275BA0;
extern short D_00275BA8;
extern volatile short D_70003B86[8];
extern short D_0028A9A0[16];

void func_001A8BE0(int arg0) {
    unsigned char **p;
    unsigned char *e;

    if (D_0028A9A0[0] == 0 && *(unsigned char *)0x70003B8D == 0) {
        p = D_00275BA0;
        D_70003B86[0] = D_00275BA8;
        while (D_70003B86[0] != 0) {
            e = *p;
            D_70003B86[0] = D_70003B86[0] - 1;
            p += 1;
            if (*(unsigned char *)e & 1) {
                switch (*(unsigned char *)(e + 3)) {
                case 1:
                    func_001A8660(arg0, e);
                    break;
                case 3:
                    func_001A8840(arg0, e);
                    break;
                case 5:
                    func_001A8970(arg0, e);
                    break;
                }
            }
        }
    }
}
