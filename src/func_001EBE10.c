// NEARMISS func_001EBE10  (vram 0x001EBE10, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-constant emit-order / instruction-scheduling permutation: CW target emits the f15(1e-6f) constant before f14(1.0f) with the paddub s0,a1 arg1-save interleaved between them; mwcc 2.3.3 and 991202 both emit f14(1.0)-first then f15(1e-6) (the same ordering the verified-100% near-identical sibling...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p

//
// NEARMISS (80.6% mwcc 2.3.3 / 74.2% mwcc 991202). Body/logic fully recovered
// and structurally identical to target. Animation/effect setup: pulls two floats
// (+0x54, +0x5C) from the global state record *D_00275C34 and calls
// func_001CFB50(&D_0081F8F0, 0, arg0, f54, f5C, 1.0f, ~1e-6f, 3.0f), then fires
// one func_001CFBE0 event on the same D_0081F8F0 object with descriptor table
// D_002564C0 (mode 1, trailing arg 0). 0x358637BD is exactly the float
// 9.999999974752427e-07. D_0081F8F0/D_002564C0 declared as arrays to force
// absolute (%hi/%lo) addressing while the small gp-rel D_00275C34 keeps its
// %gp_rel load (default sdata threshold). The idiom-13 delay-slot case (the
// func_001CFBE0 'paddub a0,s0' hoist into the jal slot) matches under 2.3.3.
//
// WALL: FP-constant emit-order / instruction-scheduling permutation. The CW
// target emits the f15 (1e-6f) constant BEFORE the f14 (1.0f) constant, with the
// arg1-save 'paddub s0,a1' interleaved between the two float setups. mwcc 2.3.3
// (and 991202) deterministically emit f14(1.0)-first then f15(1e-6) with the
// paddub before both -- the same f14-first ordering the verified-100%% sibling
// func_001EB980 (near-identical source) requires. Source-level arg reordering /
// assign-in-arg temps do not flip this. Not the clean-store nop, so 2.3.3 cannot
// fix it; this is register/scheduling-permuter territory.
extern void func_001CFB50(void *, int, int, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);
extern char *D_00275C34;
extern char D_002564C0[256];
extern char D_0081F8F0[256];

void func_001EBE10(int arg0, int arg1) {
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C),
                  1.0f, 9.999999974752427e-07f, 3.0f);
    func_001CFBE0(arg1, 1, D_002564C0, D_0081F8F0, 0);
}
