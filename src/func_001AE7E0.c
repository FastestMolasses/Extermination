// NEARMISS func_001AE7E0  (vram 0x001AE7E0, 0x120 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NOT a compiler wall — a splat/objdiff RENDERING artifact, and the object is PROVEN byte-identical to the target after linking. Mechanism: mwcc233/mwcc24 never speculate a RELOC-carrying `lui $r,%hi(sym)` into a branch delay slot, but they DO speculate a bare-CONSTANT `lui $at,0x7000`. The old sou...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Game-mode / pause classifier: polls a chain of global flags and returns a
// small state code. 0 = blocked/idle, 3 = D_008106CE active, 2 = D_008106C5 /
// D_008106B0 or the 0x800/0x10 bits of D_00810E74, 1 = the 0x100 bit set or
// D_00810E50 != 4.
//
// NOTE: D_70003B8D is the VU0 scratchpad byte at absolute 0x70003B8D. It MUST
// be spelled as a symbol, not as *(unsigned char*)0x70003B8D — the literal form
// makes mwcc speculate the bare `lui at,0x7000` into the preceding beqz delay
// slot, where the target has a nop. Requires `D_70003B8D = 0x70003B8D;` in
// config/SCUS_971.12.lcf.
extern unsigned char D_008106B0;
extern unsigned char D_008106B3;
extern unsigned char D_008106B8;
extern unsigned char D_008106B9;
extern unsigned char D_008106C5;
extern unsigned char D_008106CE;
extern unsigned char D_00810E50;
extern unsigned short D_00810E74;
extern short D_0028A9A0;
extern unsigned char D_70003B8D;

int func_001AE7E0(void) {
    if (D_008106B8 != 0) return 0;
    if (D_008106B9 != 0) return 0;
    if (D_008106CE != 0) return 3;
    if (D_008106C5 != 0 || D_008106B0 != 0) return 2;
    if (D_0028A9A0 != 0) return 0;
    if (D_70003B8D != 0) return 0;
    if ((D_00810E74 & 0x100) != 0 || D_00810E50 != 4) return 1;
    if (D_008106B3 != 0) return 0;
    if ((D_00810E74 & 0x800) != 0 || (D_00810E74 & 0x10) != 0) return 2;
    return 0;
}
