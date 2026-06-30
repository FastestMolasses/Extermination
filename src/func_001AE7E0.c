// NEARMISS func_001AE7E0  (vram 0x001AE7E0, 0x120 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.10% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single delay-slot scheduling artifact: mwcc233 hoists the raw-constant `lui at,0x7000` (abs addr 0x70003B8D high half) into the D_0028A9A0 beqz delay slot; target leaves nop. Delay/nop-fill scheduling wall, not the idiom-13 clean-store nop. volatile load qualifier and ||-merge both tried, no effe...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.10% (mwcc233). Game-mode/pause classifier: returns a small state
// code by polling a chain of global flags. 0 = blocked/idle (several flags or the
// spad gate *0x70003B8D), 3 = D_008106CE active, 2 = D_008106C5/B0 or the 0x800/0x10
// bits of D_00810E74, 1 = the 0x100 bit set or D_00810E50 != 4. The C+control flow
// is byte-correct everywhere except ONE delay slot: at the D_0028A9A0 return-0 beqz,
// mwcc233 speculatively fills the slot with the next block's raw-constant
// `lui at,0x7000` (the high half of the absolute 0x70003B8D address); the target
// leaves a nop there. This is a scheduling/delay-fill artifact on the raw-constant
// lui, NOT the clean-store-nop class (the other 6 identical beqz checks all match,
// because their successor lui carries a %hi reloc which mwcc does not speculate).
extern unsigned char D_008106B0;
extern unsigned char D_008106B3;
extern unsigned char D_008106B8;
extern unsigned char D_008106B9;
extern unsigned char D_008106C5;
extern unsigned char D_008106CE;
extern unsigned char D_00810E50;
extern unsigned short D_00810E74;
extern short D_0028A9A0;

int func_001AE7E0(void) {
    if (D_008106B8 != 0) {
        return 0;
    }
    if (D_008106B9 != 0) {
        return 0;
    }
    if (D_008106CE != 0) {
        return 3;
    }
    if (D_008106C5 != 0 || D_008106B0 != 0) {
        return 2;
    }
    if (D_0028A9A0 != 0) {
        return 0;
    }
    if (*(unsigned char *)0x70003B8D != 0) {
        return 0;
    }
    if ((D_00810E74 & 0x100) != 0 || D_00810E50 != 4) {
        return 1;
    }
    if (D_008106B3 != 0) {
        return 0;
    }
    if ((D_00810E74 & 0x800) != 0 || (D_00810E74 & 0x10) != 0) {
        return 2;
    }
    return 0;
}
