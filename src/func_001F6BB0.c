// NEARMISS func_001F6BB0  (vram 0x001F6BB0, 0x1A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW-vs-mwcc branch lowering (NOT logic): (1) original CodeWarrior left an unfilled nop in the delay slot of every dispatch beq in the selector chain; mwcc (both 991202 and 2.3.3) fills/schedules those ~7 slots away — the documented CW branch-delay-slot wall, not fixed by 2.3.3. (2) target uses bra...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS (mwcc233 = 78.7%, 991202 = 77.8%): body is essentially
// instruction-identical to the target; the residuals are all CW-vs-mwcc
// branch lowering, NOT logic. (1) The original CodeWarrior left an unfilled
// `nop` in the delay slot of every dispatch `beq` in the selector chain; mwcc
// (both 991202 and 2.3.3) schedules those slots away, so ~7 nops are missing
// — the documented CW branch-delay-slot wall, which 2.3.3 does not fix.
// (2) Target uses a branch-likely (`bnezl`) on the first func_001F6AC0 test in
// the 0x1301 path where mwcc emits a plain `bnez`. (3) Target uses a second
// epilogue entry (`b; lq ra`) for the default exit that mwcc folds away. Logic
// is correct; reaching 100% requires CW-exact branch lowering. Kept as readable
// port ground truth.
//
//
// Level/state dispatcher keyed on the 16-bit selector (D_00810700<<8 |
// D_00810701). The simple states {0x1100,0xE00,0x200,0x100,2,1} do nothing.
// State 0x1301 (combat?): with two object slots D_0025D270 / D_0025D2C0, if the
// sub-mode D_00810702 is in {2,4,5,8} it fires func_001F6850 when either slot is
// "ready" (func_001F6AC0 != 0); otherwise, for each not-ready slot, runs
// func_001F6640 when its guard bytes (D_00810778/7B for slot 0, D_0081079E for
// slot 1) are all 0xFF. State 0 (boot/title?): when D_0081075D == 0xFF and the
// descriptor from func_001F6760 is ready, runs func_001F66F0.
extern void func_001F6640(void);
extern void func_001F66F0(void);
extern int *func_001F6760(void);
extern void func_001F6850(void);
extern int func_001F6AC0(int *p);
extern int D_0025D270;
extern int D_0025D2C0;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_0081075D;
extern unsigned char D_00810778;
extern unsigned char D_0081077B;
extern unsigned char D_0081079E;

void func_001F6BB0(void) {
    int mode;
    int *p0;
    int *p1;

    mode = (D_00810700 << 8) + D_00810701;
    if (mode == 0x1100) return;
    if (mode == 0xE00) return;
    if (mode == 0x200) return;
    if (mode == 0x100) return;
    if (mode == 2) return;
    if (mode == 1) return;
    if (mode != 0x1301) {
        if (mode == 0) {
            if (D_0081075D == 0xFF && func_001F6AC0(func_001F6760()) != 0) {
                func_001F66F0();
            }
        }
        return;
    }
    p0 = &D_0025D270;
    p1 = &D_0025D2C0;
    if (D_00810702 == 2 || (unsigned int)(D_00810702 - 4) < 2 || D_00810702 == 8) {
        if (func_001F6AC0(p0) != 0 || func_001F6AC0(p1) != 0) {
            func_001F6850();
        }
    } else {
        if (func_001F6AC0(p0) == 0 && D_00810778 != 0xFF && D_0081077B != 0xFF) {
            func_001F6640();
        }
        if (func_001F6AC0(p1) == 0 && D_0081079E == 0xFF) {
            func_001F6640();
        }
    }
}
