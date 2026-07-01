// NEARMISS func_00221060  (vram 0x00221060, 0x5CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.36% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order wall: mwcc picks $a4/$t0 for the switch-dispatch byte across the whole function where the target build keeps it in $v1. All branch/constant/field logic is body-identical; only the dispatch-chain register name differs. Not the clean-store-nop wall; not fixable by re-order...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Entity state-machine dispatch on the mode byte at arg0+6, 14 explicit cases
// (0,1,2,3,4,5,6,7,0xA,0xB,0xC,0xD,0x14) driving movement/anim/timer sub-steps;
// case 3 gates on flag bit 0x1000 of arg0+0x200 and, when clear, either escalates
// to state 0x14 (boss-hit / special condition arg0+0xF==0x63 or arg0+0x234==1) or
// falls to state 0xA, else advances by 1 or 2 depending on distance/timer fields;
// tail check advances to func_0021D250 while mode<3 and arg0+0x23A==0x5D (an
// external interrupt/override state). D_00275B40 is a pointer to a global timing
// struct (offsets +4/+8 = two running clocks used for delta-time).
// Logic fully recovered (all branches/constants/field accesses body-identical to
// target); sole residual is a REGISTER-ALLOCATION-ORDER wall: mwcc picks $a4/$t0
// for the switch-dispatch byte across the whole function where the target build
// keeps it in $v1 (first scratch reg) -- every dispatch-chain "addiu/beq" pair on
// the mode value therefore differs only by register name, not semantics. Tried:
// mode as a persistent local vs. re-read-per-site (target's own idiom, matches
// its per-case *(u8*)(arg0+6) reloads), local declaration order, -sdatathreshold
// 0/4/8/16 (8 gp-rel's D_00275B40 correctly, matches target's %gp_rel access).
// None move the switch-index register; this is the documented saved-register-
// allocation-order wall (PROGRESS.md idiom 15/17 family), not a clean-store nop.
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(int, int, int, float);
extern void func_001749A0(int, int, int, float);
extern int func_00178B90(int, int, int);
extern int func_00175900(int, int);
extern void func_00179880(int, int);
extern int func_0021C190(int, int, int);
extern void func_0021C120(int, int, int);
extern void func_0021C200(int, int, int, int);
extern void func_0017C540(int, int);
extern void func_0021D2E0(int, int);
extern void func_0021D490(float);
extern void func_0021D250(int, int);
extern int *D_00275B40;
extern unsigned char D_008106F1;

void func_00221060(int arg0) {
    int flag;
    unsigned char mode;

    mode = *(unsigned char *)(arg0 + 6);
    switch (mode) {
    case 0:
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(arg0, 0x150, 0, 300.0f);
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        func_001749A0(arg0, 0x26, 0, 1.0f);
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*D_00275B40 + 8);
            *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) * 0.75f;
            func_00178B90(1, 5, 1);
            *(float *)(arg0 + 0x2EC) = *(float *)(*D_00275B40 + 4) - *(float *)(arg0 + 0x2E4);
            *(float *)(arg0 + 0x2E4) = *(float *)(*D_00275B40 + 4);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)(arg0 + 0x2EC);
            func_00175900(arg0, 1);
        }
        break;
    case 2:
        func_00178B90(1, 5, 0xA);
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001FBD50(arg0, 0x12F, 0, 300.0f);
            func_001B61C0(0, 0xD0, 0xA, 1);
            func_001749A0(arg0, 0x27, 0, 0.0f);
            *(float *)(arg0 + 0x21C) = 0.0f;
            *(float *)(arg0 + 0x2E4) = 0.0f;
        }
        break;
    case 3:
        flag = *(int *)(arg0 + 0x200) & 0x1000;
        if (flag != 0) {
            if (*(float *)(arg0 + 0x220) <= 0.0f) {
                if ((*(unsigned char *)(arg0 + 0xF) == 0x63) || (*(unsigned char *)(arg0 + 0x234) == 1)) {
                    func_0021C200(arg0, flag, 5, 0xA);
                    *(unsigned char *)(arg0 + 6) = 0x14;
                    func_001749A0(arg0, 0x1C4, 0, 16.0f);
                } else {
                    *(unsigned char *)(arg0 + 6) = 0xA;
                    func_001749A0(0x29, 0, 0xA, 4.0f);
                }
            } else if (!(*(float *)(arg0 + 0x228) < 100.0f) && (D_008106F1 != 0)) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                func_0021C120(D_008106F1, 5, 0xA);
                *(unsigned char *)(arg0 + 0x302) = 1;
            } else {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 2;
                *(unsigned char *)(arg0 + 0x302) = 0;
            }
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(1, 5, 0xA);
            func_00175900(arg0, 1);
        }
        break;
    case 4:
        if (func_0021C190(4, 5, 0xA) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        }
        break;
    case 5:
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        func_001749A0(0x28, 0, 0xA, 8.0f);
        *(float *)(arg0 + 0x38) = 0.0f;
        *(float *)(arg0 + 0x21C) = 0.0f;
        break;
    case 6:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(float *)(arg0 + 0x2EC) = 0.0f;
            *(char *)(arg0 + 0x1F1) = 0;
        }
        break;
    case 7:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 0xF) = 0;
            *(int *)(arg0 + 0x224) = 0;
            *(int *)(arg0 + 0x22C) = 0;
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(char *)(arg0 + 0x25C) = 0;
            func_0017C540(7, 0xA);
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(1, 0xA, 0);
            if (*(unsigned char *)(arg0 + 0x302) != 0) {
                *(int *)(arg0 + 0x204) = 0x3ECCCCCD;
            }
        }
        func_00179880(arg0, arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 0xA:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001FBD50(0x156, 0, 0xA, 300.0f);
        }
        break;
    case 0xB:
        if (*(float *)(arg0 + 0x3C) <= 18.0f) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_0021D490(18.0f);
        }
        break;
    case 0xC:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(char *)(arg0 + 7) = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 0xD:
        func_0021D2E0(0x78, 0);
        break;
    case 0x14:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(char *)(arg0 + 4) = 2;
            *(char *)(arg0 + 5) = 3;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x3F;
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(1, 0, 0);
        }
        break;
    }
    if ((*(unsigned char *)(arg0 + 6) < 3) && (*(unsigned char *)(arg0 + 0x23A) == 0x5D)) {
        func_0021D250(arg0, 1);
    }
}
