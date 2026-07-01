// NEARMISS func_0016EF50  (vram 0x0016EF50, 0x5D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.09% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Compiler scheduling/register-choice wall (idiom-13 family): the drift-add pair (arg0+0xB0/+0xB8 += arg0+0x2E0/+0x2E8, appears 3x) has target loading the RHS operand before the LHS while keeping the same add.s operand order; swapping the C addition operand order only flips the add.s arguments (wro...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Entity state-machine dispatch on mode byte arg0+6 (cases 0, 1, 0xA, 0xB,
// 0x14, 0x15, 0x16), driving a "charge/aim/fire" cycle with an inner
// sub-state at arg0+7:
//  case 0: init drift velocity (arg0+0x2E0/+0x2E8, magnitude 0.1315789..
//   scaled by cos/sin of current yaw arg0+0xC4) and pick the wind-up anim id
//   (0x145/0x146 depending on arg0+0x2F1 facing flag).
//  case 1: once flag bit 0x1000 of arg0+0x200 sets (wind-up anim done),
//   advance to state 0xA, reset sub-state, snapshot the aim yaw (arg0+0x218)
//   into arg0+0xC4, and kick off the follow-up anim via func_00188610;
//   otherwise drift position (arg0+0xB0/+0xB8) by the case-0 velocity.
//  case 0xA falls into 0xB: func_001821E0 is a per-tick gate (nonzero ->
//   return immediately, no tail run). Sub-state 0 (first tick): call
//   func_001751A0, snapshot arg0+0x23F into arg0+0x25C, then branch on
//   arg0+0x24C (attack-type id): 0 builds a transform via func_001026A0 and
//   moves to state 5/mode 0 (a knockback/stagger state), 2/3 look up a
//   per-type duration from D_00248670[arg0+0x23F] into arg0+0x26C/+0x204,
//   flip a facing flag (arg0+0x2F1) and advance the sub-state; any other
//   attack-type id leaves state unchanged. Sub-state 1: once flag 0x1000
//   sets, either escalate to state 0x14 (arg0+0x23B != 0x39) or restart the
//   cycle via func_00188610; otherwise recompute the delta-time-scaled timer
//   (arg0+0x38, arg0+0x21C) against the global clock *D_00275B40 and call
//   func_00179560.
//  case 0x14: recovery-anim setup mirroring case 0, computing arg0+0x218 (the
//   aim yaw +-90 degrees) and a signed drift velocity depending on facing.
//  case 0x15: once flag 0x8000 clears, run one func_001764E0 tick at the aim
//   yaw (arg0+0x218) then restore arg0+0xC4; always drifts by the case-0x14
//   velocity.
//  case 0x16: once flag 0x1000 sets, finalize (anim_eval_skeleton, snap yaw
//   to arg0+0x218, func_00174AB0, reset mode/sub-mode to 0) and return
//   directly (skips the tail); otherwise drifts by velocity.
//  Tail (all non-return cases): arg0+0xB4 -= 0.2, func_00175900(arg0,0),
//  func_001796C0(arg0).
//
// Logic and structure fully recovered. Residuals are compiler-internal
// scheduling/register-choice artifacts, not logic gaps: (1) the drift-add
// pair `arg0+0xB0/+0xB8 += arg0+0x2E0/+0x2E8` (appears 3x) loads the RHS
// operand before the LHS in the target (mwcc233 here loads LHS first) while
// keeping the same add.s operand order -- tried swapping the C addition
// order, which only flips the add.s operands themselves (wrong), so this is
// not C-addressable; (2) a handful of literal-register picks (a0 vs a1, v0
// vs v1) in the arg0+0x24C/arg0+0x23F dispatch and the D_00248670[idx]
// array-index reload differ by register name only, same values/instructions.
// Both are the documented CW/mwcc scheduling wall family (PROGRESS.md idiom
// 13), not the clean-store-nop case, so 2.3.3 does not fully resolve them.
extern void anim_eval_skeleton(char *p);
extern int func_001026A0(void *dst, void *src, void *m);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_001749A0(unsigned char *e, int id, int z, float f);
extern void func_00174AB0(char *p);
extern void func_001751A0(void *p, int idx);
extern int func_00175900(unsigned char *e, int flag);
extern void func_001764E0(char *p);
extern void func_00179560(char *p);
extern void func_001796C0(char *p);
extern int func_001821E0(unsigned char *e, int a1, int a2);
extern int func_00188610(unsigned char *e);
extern float func_001B1470(float);
extern float D_00248670[];
extern int *D_00275B40;

void func_0016EF50(unsigned char *arg0) {
    unsigned char st;

    st = arg0[6];
    switch (st) {
    case 0:
        arg0[6] = st + 1;
        if (arg0[0x2F1] == 0) {
            func_001749A0(arg0, 0x145, 0, 8.0f);
        } else {
            func_001749A0(arg0, 0x146, 0, 8.0f);
        }
        *(int *)0x70003A20 = 0x3E06BCA2;
        *(float *)(arg0 + 0x2E0) = *(float *)0x70003A20 * func_0011E2A8(*(float *)(arg0 + 0xC4));
        *(float *)(arg0 + 0x2E8) = *(float *)0x70003A20 * func_0011DE90(*(float *)(arg0 + 0xC4));
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6] = 0xA;
            arg0[7] = 0;
            arg0[0x1F1] = 1;
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
            func_001749A0(arg0, func_00188610(arg0), 0, 0.0f);
        } else {
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)(arg0 + 0x2E0);
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)(arg0 + 0x2E8);
        }
        break;
    case 0xA:
        arg0[6] = st + 1;
        *(float *)(arg0 + 0x38) = 0.0f;
        /* fallthrough */
    case 0xB:
        if (func_001821E0(arg0, 0, st) != 0) {
            return;
        }
        switch (arg0[7]) {
        case 0:
            func_001751A0(arg0, 0);
            arg0[0x25C] = arg0[0x23F];
            switch (*(int *)(arg0 + 0x24C)) {
            case 0:
                *(int *)0x700038A0 = 0;
                *(int *)0x700038A4 = 0;
                *(float *)0x700038A8 = 5.0f;
                *(float *)0x700038AC = 1.0f;
                func_001026A0(arg0 + 0xB0, arg0 + 0xD0, (void *)0x700038A0);
                *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 0.2f;
                arg0[0x25C] = 1;
                arg0[5] = 5;
                arg0[6] = 0;
                arg0[0x1F0] = 0xB;
                break;
            case 2:
                if (arg0[0x23F] == 3) {
                    func_001749A0(arg0, 0x14A, 0, 1.0f);
                } else {
                    func_001749A0(arg0, 0x148, 0, 1.0f);
                }
                *(float *)(arg0 + 0x26C) = D_00248670[arg0[0x23F]];
                *(float *)(arg0 + 0x204) = D_00248670[arg0[0x23F]];
                arg0[0x2F1] = 0;
                arg0[7] = arg0[7] + 1;
                break;
            case 3:
                if (arg0[0x23F] == 3) {
                    func_001749A0(arg0, 0x14B, 0, 1.0f);
                } else {
                    func_001749A0(arg0, 0x149, 0, 1.0f);
                }
                *(float *)(arg0 + 0x26C) = D_00248670[arg0[0x23F]];
                *(float *)(arg0 + 0x204) = D_00248670[arg0[0x23F]];
                arg0[0x2F1] = 1;
                arg0[7] = arg0[7] + 1;
                break;
            default:
                goto sub_done;
            }
            *(float *)(arg0 + 0x38) = 0.0f;
            *(float *)(arg0 + 0x21C) = 0.0f;
            break;
        case 1:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                if (arg0[0x23B] != 0x39) {
                    arg0[6] = 0x14;
                    arg0[0x1F1] = 2;
                    break;
                }
                arg0[7] = 0;
                func_001749A0(arg0, func_00188610(arg0), 0, 12.0f);
                break;
            }
            *(float *)(arg0 + 0x204) = *(float *)(arg0 + 0x26C);
            *(float *)(arg0 + 0x38) = *(float *)(*D_00275B40) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*D_00275B40);
            func_00179560((char *)arg0);
            break;
        }
    sub_done:
        break;
    case 0x14:
        arg0[6] = st + 1;
        *(int *)0x70003A20 = 0x3E06BCA2;
        if (arg0[0x2F1] == 0) {
            func_001749A0(arg0, 0x14C, 0, 8.0f);
            *(float *)(arg0 + 0x218) = func_001B1470(*(float *)(arg0 + 0xC4) - 1.5707964f);
            *(float *)(arg0 + 0x2E0) = -*(float *)0x70003A20 * func_0011DE90(*(float *)(arg0 + 0xC4));
            *(float *)(arg0 + 0x2E8) = *(float *)0x70003A20 * func_0011E2A8(*(float *)(arg0 + 0xC4));
        } else {
            func_001749A0(arg0, 0x14D, 0, 8.0f);
            *(float *)(arg0 + 0x218) = func_001B1470(1.5707964f + *(float *)(arg0 + 0xC4));
            *(float *)(arg0 + 0x2E0) = *(float *)0x70003A20 * func_0011DE90(*(float *)(arg0 + 0xC4));
            *(float *)(arg0 + 0x2E8) = *(float *)0x70003A20 * -func_0011E2A8(*(float *)(arg0 + 0xC4));
        }
        break;
    case 0x15:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = st + 1;
            *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0xC4);
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
            func_001764E0((char *)arg0);
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x26C);
        }
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)(arg0 + 0x2E0);
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)(arg0 + 0x2E8);
        break;
    case 0x16:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            anim_eval_skeleton((char *)arg0);
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
            func_00174AB0((char *)arg0);
            arg0[5] = 0;
            arg0[6] = 0;
            arg0[0x1F0] = 0;
            return;
        }
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)(arg0 + 0x2E0);
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)(arg0 + 0x2E8);
        break;
    }
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 0.2f;
    func_00175900(arg0, 0);
    func_001796C0((char *)arg0);
}
