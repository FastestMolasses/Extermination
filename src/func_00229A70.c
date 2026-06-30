// NEARMISS func_00229A70  (vram 0x00229A70, 0x18C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two CodeWarrior codegen artifacts: (1) arg-eval/scheduling order on the leading func_001FE9A0 call (target fills jal delay slot with the last addr arg; mwcc schedules li a0,1 there); (2) CW emits a duplicated early-return epilogue for each `D_00275C58==0 -> return 0` path that mwcc merges into th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 78.6% (mwcc233). Logic fully recovered; the two residuals are pure
// CodeWarrior codegen artifacts: (1) arg-eval/scheduling order on the leading
// func_001FE9A0 call -- the target fills the jal delay slot with the last
// address arg (addiu a2,s0,84) and emits li a0,1 first, while mwcc schedules
// li a0,1 into the slot; (2) CW emits a DUPLICATED early-return epilogue for
// each `D_00275C58==0 -> return 0` path (a separate `lq ra` / branch that
// re-restores into the shared tail), which mwcc merges into the single shared
// epilogue (known CW branch-lowering / dead-return-block wall, not the
// idiom-13 clean-store nop). The body, the 6-arg func_001FEC20 calls (state,0,
// &D_002738E0,0,1,&D_00821500 with args 5/6 in t0/t1), the gp-rel D_00275C58
// load and the case-2 priority logic all match.
//
// Two-stage async-load state machine, sibling of func_002283B0/func_00228530.
// Guard: func_001FE9A0(1, &+0x50, &+0x54); on 0, return. state(+4)==0: issue
// step 0 via func_001FEC20(0,0,&D_002738E0,0,1,&D_00821500); on -1 honor the
// busy flag D_00275C58 (==0 -> return, else clear +0x1A), else +0x1A = (result
// != 0); func_001FE8D0(); advance state to 1. state==1: same for step 1, flag
// into +0x1B, advance to 2. state==2: +3 = ((+0x1A||+0x1B) ? 1 : 2); reset
// state to 0. Returns 0.
//
// Built with mwcc 2.3.3 (best of 78.6% vs 65% on 991202); residual is the
// CW-specific scheduling + duplicated-epilogue layout, not fixable by 2.3.3.
extern int func_001FE9A0(int, char *, char *);
extern int func_001FEC20(int, int, void *, int, int, void *);
extern void func_001FE8D0(void);
extern int D_002738E0[];
extern int D_00821500[];
extern int D_00275C58;

int func_00229A70(unsigned char *arg0) {
    int v0;

    if (func_001FE9A0(1, (char *)(arg0 + 0x50), (char *)(arg0 + 0x54)) == 0) {
        return 0;
    }
    switch (arg0[4]) {
    case 0:
        v0 = func_001FEC20(0, 0, &D_002738E0, 0, 1, &D_00821500);
        if (v0 == -1) {
            if (D_00275C58 == 0) {
                return 0;
            }
            arg0[0x1A] = 0;
        } else {
            if (v0 != 0) {
                arg0[0x1A] = 1;
            } else {
                arg0[0x1A] = 0;
            }
        }
        func_001FE8D0();
        arg0[4] = 1;
        return 0;
    case 1:
        v0 = func_001FEC20(1, 0, &D_002738E0, 0, 1, &D_00821500);
        if (v0 == -1) {
            if (D_00275C58 == 0) {
                return 0;
            }
            arg0[0x1B] = 0;
        } else {
            if (v0 != 0) {
                arg0[0x1B] = 1;
            } else {
                arg0[0x1B] = 0;
            }
        }
        func_001FE8D0();
        arg0[4] = 2;
        return 0;
    case 2:
        if (arg0[0x1A] != 0) {
            arg0[3] = 1;
        } else if (arg0[0x1B] != 0) {
            arg0[3] = 1;
        } else {
            arg0[3] = 2;
        }
        arg0[4] = 0;
        return 0;
    }
    return 0;
}
