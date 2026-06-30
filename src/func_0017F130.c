// NEARMISS func_0017F130  (vram 0x0017F130, 0x90 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.72% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc-vs-CW branch lowering + FP move order: CW lowers the mode-4 `return func()!=0` as an explicit branch (beqz/b/li) but both mwcc builds emit a movz conditional-move; additionally CW emits the two FP arg moves f13-before-f12 vs mwcc's f12-before-f13. Body, both float constants, and the mode-17 ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Dispatch on the global mode byte D_00810700.
//  mode 4 : return (func_0017E6E0(0.0f, 1.0f) != 0).
//  mode 17: if arg0->field_0x316 != 0, return (func_0017E6E0(19.0f, 1.0f) != 0).
//  otherwise / on a zero call result: return 0.
// NEARMISS (84.7% vs build/expected/func_0017F130.o, mwcc 2.3.3): full logic and
// the mode-17 path match; both float constants (1.0f, 19.0f) and the +0x316
// guard are exact. Two residuals, both compiler-version codegen differences:
//  (1) for mode 4, CodeWarrior lowers `return func()!=0` as an explicit
//      branch (beqz / b ; li v0,1) where both our mwcc builds emit a movz
//      conditional-move.
//  (2) the two FP argument moves for the mode-4 call are emitted f13-before-f12
//      by CW vs f12-before-f13 by mwcc (the f13-temp flip idiom did not change
//      mwcc's order here). Logic is exact.
extern unsigned char D_00810700;
extern int func_0017E6E0(float, float);

int func_0017F130(char *arg0) {
    if (D_00810700 == 4) {
        if (func_0017E6E0(0.0f, 1.0f) != 0) {
            return 1;
        }
        return 0;
    }
    if (D_00810700 == 0x11) {
        if (*(unsigned char *)(arg0 + 0x316) != 0) {
            if (func_0017E6E0(19.0f, 1.0f) != 0) {
                return 1;
            }
        }
    }
    return 0;
}
