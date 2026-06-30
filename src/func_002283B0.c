// NEARMISS func_002283B0  (vram 0x002283B0, 0x138 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.81% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-coloring permutation: body byte-identical, only 2 arg-register diffs remain (dispatch constant 1 in $t0 vs $v0; 5th func_001FEC20 arg &D_00821500 in $t1 vs $t0). Permuter territory. 99.8% on mwcc233.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 99.8% (mwcc 2.3.3; 991202 reaches 84.3%). Per-state stream/clip driver
// gated on func_001FE9A0 readiness (a0=1, the s0+0x50/0x54 channel pair). If the
// gate returns 0 the func returns 0. Otherwise dispatch on the state byte at
// s0+2 (states 0/1/2; sibling of the matched func_00226B90/func_0022A020 family):
//   state 0: step func_001FEC20(*(s0+0x48), *(s0+0x4C), &D_002738E0, 0, &D_00821500)
//            (a 5-arg call; the 5th pointer lands in the a4/t0..t1 arg slot). If
//            it reports <0 fall to the shared busy epilogue; if it reports 0 run
//            func_00121A28(s0+0x6C, 0, 8) and return 1; otherwise advance state to 1.
//   state 1: r = sub_BASCUS_97112_DS00_00_EX_DATA_00_3(s0); switch(r): 3 -> return 3,
//            2 -> return 2, 1 -> advance state to 2, 0 -> fall through. The 2/3 arms
//            use the target's beql branch-likely with the return value (paddub) in
//            the delay slot -- reproduced by the ascending-case switch.
//   state 2: return 1.
// Shared epilogue: return D_00275C58 ? 2 : 0  (the movz v0,zero,D_00275C58 idiom).
//
// WALL: register-coloring permutation only -- body is byte-identical. Two residual
// arg-register diffs: (1) the state-dispatch constant 1 sits in $t0 in the target
// vs $v0 here; (2) &D_00821500 (the 5th func_001FEC20 arg) sits in $t1 ($9) in the
// target vs $t0 ($8/a4) here. Pure allocation order -- permuter territory.
extern int func_001FE9A0(int, char *, char *);
extern int func_001FEC20(int, int, int, int, int);
extern void func_00121A28(int, int, int);
extern int sub_BASCUS_97112_DS00_00_EX_DATA_00_3(int);
extern int D_00275C58;
extern int D_002738E0[];
extern int D_00821500[];

int func_002283B0(char *arg0) {
    char *s0;
    int r;

    s0 = arg0;
    if (func_001FE9A0(1, s0 + 0x50, s0 + 0x54) == 0) {
        return 0;
    }
    switch (*(unsigned char *)(s0 + 2)) {
    case 0:
        r = func_001FEC20(*(int *)(s0 + 0x48), *(int *)(s0 + 0x4C), (int)D_002738E0, 0, (int)D_00821500);
        if (r < 0) {
            break;
        }
        if (r == 0) {
            func_00121A28((int)(s0 + 0x6C), 0, 8);
            return 1;
        }
        *(unsigned char *)(s0 + 2) = 1;
        break;
    case 1:
        r = sub_BASCUS_97112_DS00_00_EX_DATA_00_3((int)s0);
        switch (r) {
        case 0:
            break;
        case 1:
            *(unsigned char *)(s0 + 2) = 2;
            break;
        case 2:
            return 2;
        case 3:
            return 3;
        }
        break;
    case 2:
        return 1;
    }
    if (D_00275C58 != 0) {
        return 2;
    }
    return 0;
}
