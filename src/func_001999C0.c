// NEARMISS func_001999C0  (vram 0x001999C0, 0x290 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation ($s0/$s1 hold scratch/result swapped vs. target) plus per-branch dead-code scheduling: each early-exit func_001DB800() call site in the target still materializes a leftover dead constant in a different slot than mwcc. All logic, branch conditions, and call argument v...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Weapon-switch UI hook. arg0 is the player/HUD actor, arg1 is a "commit"
// flag (0 = just clear the crosshair/preview widgets and report which
// tracker rows to reset; nonzero = actually arm/engage the selected weapon
// system and run its full setup). If arg1 != 0, freeze input first
// (func_001D2040(1,0)). Dispatches on the global aim-option/weapon-family
// byte D_00810CA4:
//   2 (missile launcher): arg1==0 -> func_001DB800() (clear); else
//     func_001DB830(1,0xFF) + sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER(1) +
//     func_00199770(arg0) (target scan/lock).
//   1 (delta auto-sight): arg1==0 -> func_001DB800(); else
//     func_001DB9D0(1,2,1.0f) + func_001DC610(1) + func_001DC890(1, zoom)
//     where zoom = *(float*)(arg0+0x20+0x1F0+0x24).
//   0 (tactical advanced): arg1==0 -> func_001DB800(); else
//     func_001DB9D0(1,2,1.0f) + sub_DELTA_AUTO_SIGHT_SYS(1) +
//     func_001DC020(1, zoom).
//   default: read D_00810CA7 (weapon subtype):
//     8 (tactical): arg1==0 -> func_001DB800(); else
//       func_001DB9D0(1,2,1.0f) + sub_TACTICAL_ADVANCED(1) +
//       func_001DBCB0(1, D_00810248) + func_001D2610(D_00810248);
//       result flag = 1.
//     9 (night vision): arg1==0 -> func_001DB800() then arm two auto-scale
//       cues func_0021B9A0(4,0,1000000.0f)/func_0021B9A0(5,0,10000000.0f)
//       and func_001D2830(1); else func_001DB830(1,0xFF) +
//       sub_NIGHT_VISION_SYSTEM(1).
//     other: arg1==0 -> func_001DB800().
//   Then always: if D_00810CA5==6, func_0022E7F0(&D_008102B0) (highlight
//   hook). Returns 1 only for the tactical(8)+arg1!=0 path, else 0.
//
// NEARMISS (mwcc233 83.20%, mwcc 991202 81.70%). Logic and control-flow
// fully recovered -- every store/branch condition/call arg matches; the
// residual is (1) a register-coloring permutation ($s0/$s1 hold
// scratch/result the opposite way from the target across the whole
// function) and (2) per-branch dead-code scheduling: each early-exit
// func_001DB800() call site in the target still materializes a leftover
// float/int constant (dead `lui`/`mtc1`/`addiu` from an earlier
// computation that this branch no longer uses) in a slightly different
// slot than mwcc chooses here. func_001DB800/func_001DB830/func_001DBCB0/
// func_001D2610/sub_TACTICAL_ADVANCED/sub_DELTA_AUTO_SIGHT_SYS/
// sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER/sub_NIGHT_VISION_SYSTEM signatures
// are taken from their own matched/near-miss source files; func_001DB9D0
// and func_0021B9A0 remain undecompiled (INCLUDE_ASM) -- their extern
// declarations here are inferred from this call site's live-register setup
// only. Tried: caching the zoom float once vs. reloading it at each use
// (reload is correct -- matches target); declaration-order swap of
// scratch/result (no effect on mwcc's allocation). Parked as a
// register-coloring + dead-code-scheduling wall.
extern void func_001D2040(int a0, int a1);
extern void func_001DB800(void);
extern void func_001DB830(int a0, int a1);
extern void sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER(int a0);
extern void func_00199770(char *arg0);
extern void func_001DB9D0(int a0, int a1, float a2);
extern void func_001DC610(int a0);
extern void func_001DC890(int a0, float a1);
extern void sub_DELTA_AUTO_SIGHT_SYS(int a0);
extern void func_001DC020(int a0, float a1);
extern void sub_TACTICAL_ADVANCED(int a0);
extern void func_001DBCB0(int a0, float a1);
extern void func_001D2610(float a0);
extern void func_0021B9A0(int a0, float a1, float a2);
extern void func_001D2830(int a0);
extern void sub_NIGHT_VISION_SYSTEM(int a0);
extern void func_0022E7F0(void *a0);
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA7;
extern float D_00810248;
extern int D_008102B0;

int func_001999C0(char *arg0, int arg1) {
    char *scratch;
    int result;

    scratch = (char *)(*(int *)(arg0 + 0x20) + 0x1F0);
    result = 0;
    if (arg1 != 0) {
        func_001D2040(1, 0);
    }
    if (D_00810CA4 == 2) {
        if (arg1 == 0) {
            func_001DB800();
        } else {
            func_001DB830(1, 0xFF);
            sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER(1);
            func_00199770(arg0);
        }
    } else if (D_00810CA4 == 1) {
        if (arg1 == 0) {
            func_001DB800();
        } else {
            func_001DB9D0(1, 2, 1.0f);
            func_001DC610(1);
            func_001DC890(1, *(float *)(scratch + 0x24));
        }
    } else if (D_00810CA4 == 0) {
        if (arg1 == 0) {
            func_001DB800();
        } else {
            func_001DB9D0(1, 2, 1.0f);
            sub_DELTA_AUTO_SIGHT_SYS(1);
            func_001DC020(1, *(float *)(scratch + 0x24));
        }
    } else if (D_00810CA7 == 8) {
        if (arg1 == 0) {
            func_001DB800();
        } else {
            func_001DB9D0(1, 2, 1.0f);
            sub_TACTICAL_ADVANCED(1);
            func_001DBCB0(1, D_00810248);
            func_001D2610(D_00810248);
            result = 1;
        }
    } else if (D_00810CA7 == 9) {
        if (arg1 == 0) {
            func_001DB800();
            func_0021B9A0(4, 0.0f, 1000000.0f);
            func_0021B9A0(5, 0.0f, 10000000.0f);
            func_001D2830(1);
        } else {
            func_001DB830(1, 0xFF);
            sub_NIGHT_VISION_SYSTEM(1);
        }
    } else if (arg1 == 0) {
        func_001DB800();
    }
    if (D_00810CA5 == 6) {
        func_0022E7F0(&D_008102B0);
    }
    return result;
}
