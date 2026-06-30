// NEARMISS func_001B11E0  (vram 0x001B11E0, 0x58 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 79.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation/scheduling permutation: identical opcodes (lbu/sra/sll/andi/lui/addiu/sllv/and/movz), but mwcc assigns the temporaries to different registers (a1 vs a2 cascade) and emits 'addiu v0,1; sllv a0,..' before the lw whereas target emits it after. movz matches. Not the clean-store no...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Bit-test in a 2D table: row selected by global D_00810700 (stride 0x20
// bytes), column word = (idx>>5)*4, bit = idx&0x1F. Returns 1 if the bit
// is set, else 0. Returns 0 immediately if (arg0 & 0xFF) == 0.
// NEARMISS: logic byte-exact; residual is pure register-allocation /
// instruction-scheduling permutation (same opcodes, mwcc materializes
// 1<<bit and the table address into different registers and emits the
// sllv before the lw rather than after). Permuter-class wall.
extern unsigned char D_00810700;
extern char D_00810860;

int func_001B11E0(int arg0) {
    int v0 = arg0 & 0xFF;
    if (v0 == 0) {
        return 0;
    }
    if ((*(int *)(&D_00810860 + (D_00810700 << 5) + (v0 >> 5) * 4) & (1 << (arg0 & 0x1F)))) {
        return 1;
    }
    return 0;
}
