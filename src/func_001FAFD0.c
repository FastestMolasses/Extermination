// NEARMISS func_001FAFD0  (vram 0x001FAFD0, 0xD8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 70.93% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Branch-lowering + delay-slot-scheduling permutation: distinct early-return epilogue (ra-restore-in-slot vs merged epilogue), v1=0x18 reuse with inverted bne+b at the 0x15 test, and jal func_001FABF0 arg-slot fill order. Body/logic 100% correct. NOT the clean-store nop wall (233=70.9% regresses; 9...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 84.4% (mwcc 991202). Gated state-event dispatcher. Runs only when
// the byte at 0x70003B90 is set; bails immediately if 0x70003B8D is set.
// When D_008104E4 == 1 (a mode flag) and the current value D_00282178 is below
// 0x1D and the level/scene id D_00810D38 is none of {0xB,0xC,0x17}: unless the
// sub-state D_00810700 is 0x15 or D_00282178 is already 0x18, it resets via
// func_001FAAC0(0, D_00282178) then arms func_001FABF0(0,0x18,0xB4,1). In the
// other mode, when D_00282178 == 0x18 it fires func_001FAE70(1, D_00282178).
//
// Body/logic verified correct. Residual is a branch-lowering + scheduling
// permutation: (1) the early 0x70003B8D return uses a distinct ra-restore-in-
// delay-slot epilogue in the target vs. the merged common epilogue mwcc emits;
// (2) the target reuses v1=0x18 across the 0x15 and ==0x18 tests with an
// inverted bne+b shape; (3) the func_001FABF0 arg setup fills the jal delay slot
// differently. All register-alloc/scheduling class -> permuter, not the
// clean-store nop wall (233 regresses to 70.9% here, so 991202 is the build).
extern void func_001FAAC0(int, int);
extern void func_001FABF0(int, int, int, int);
extern void func_001FAE70(int, int);
extern int D_00282178;
extern unsigned char D_008104E4;
extern unsigned char D_00810700;
extern int D_00810D38;

void func_001FAFD0(void) {
    if (*(unsigned char *)0x70003B90 != 0) {
        if (*(unsigned char *)0x70003B8D != 0) {
            return;
        }
        if (D_008104E4 == 1) {
            if (D_00282178 < 0x1D && D_00810D38 != 0xB && D_00810D38 != 0xC && D_00810D38 != 0x17) {
                if (D_00810700 == 0x15) {
                    return;
                }
                if (D_00282178 != 0x18) {
                    func_001FAAC0(0, D_00282178);
                    func_001FABF0(0, 0x18, 0xB4, 1);
                }
            }
        } else if (D_00282178 == 0x18) {
            func_001FAE70(1, D_00282178);
        }
    }
}
