// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Byte-matched (lane DFIX2 2026-10-02, was NEARMISS 90.09%) and corrected against the
// original instructions (port docs/LEVEL12_PORT.md section 0; docs/FINDINGS.md "NEARMISS
// body corrections from the level side-track lanes, second round"). Corrections: in
// states 0xA / 0xB sub-state 0 a +0x24C other than 0, 2 and 3 still clears +0x38 and
// +0x21C; the table word D_00248670[+0x23F] is read once for +0x26C and +0x204; state
// 0x15 calls func_001764E0 again after the drift; state 0x16 calls func_001764E0 on both
// paths and its done path runs the common tail; func_001751A0 takes self only. Levers:
// the +0x24C dispatch as an if / else-if chain tested 0, 3, 2; the scratch vector passed
// as the relocated D_700038A0; `+=` for the drift and for the -0.2 step.
//
// Entity state machine on the mode byte +6 (cases 0, 1, 0xA, 0xB, 0x14, 0x15, 0x16) with a
// sub-state at +7:
//  case 0: starts clip 0x145 / 0x146 (by +0x2F1) and sets the drift step +0x2E0 / +0x2E8
//   from the scratch constant 0x70003A20 times the two trig kernels of the heading +0xC4.
//  case 1: once bit 0x1000 of +0x200 is set, mode 0xA, sub-state 0, +0x1F1 = 1, heading
//   = +0x218 and the clip func_00188610 picks; otherwise drifts +0xB0 / +0xB8 by the step.
//  case 0xA (sets mode 0xB and +0x38 = 0) falls into 0xB: func_001821E0(self, 0, mode)
//   nonzero returns at once (no tail). Sub-state 0: func_001751A0(self), +0x25C = +0x23F,
//   then on +0x24C: 0 sets +0xB0 = the +0xD0 matrix applied to (0, 0, 5, 1) (func_001026A0),
//   steps +0xB4 by -0.2 and sets +0x25C = 1, mode 5 / 0, +0x1F0 = 0xB; 3 and 2 start clip 0x14B / 0x149
//   or 0x14A / 0x148 (by +0x23F == 3), copy D_00248670[+0x23F] to +0x26C and +0x204,
//   set +0x2F1 = 1 / 0 and advance the sub-state; then (every +0x24C) +0x38 = +0x21C = 0.
//   Sub-state 1: bit 0x1000 set -> mode 0x14 with +0x1F1 = 2 (when +0x23B != 0x39) or
//   sub-state 0 and the clip func_00188610 picks; otherwise +0x204 = +0x26C, +0x38 = clock
//   - +0x21C, +0x21C = clock (the float behind *D_00275B40) and func_00179560(self).
//  case 0x14: mode 0x15; starts clip 0x14C / 0x14D (by +0x2F1), sets +0x218 = the heading
//   -/+ pi/2 (func_001B1470) and a step perpendicular to the heading.
//  case 0x15: with bit 0x8000 clear, mode 0x16 and one func_001764E0 at heading +0x218
//   (+0x26C keeps the heading meanwhile); then drift and func_001764E0.
//  case 0x16: bit 0x1000 set -> anim_eval_skeleton, heading = +0x218, func_00174AB0, mode
//   bytes +5 / +6 / +0x1F0 = 0; else drift; then func_001764E0.
//  Tail: +0xB4 += -0.2, func_00175900(self, 0), func_001796C0(self).
extern void anim_eval_skeleton(char *p);
extern int func_001026A0(void *dst, void *src, void *m);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_001749A0(unsigned char *e, int id, int z, float f);
extern void func_00174AB0(char *p);
extern void func_001751A0(unsigned char *self);
extern int func_00175900(unsigned char *e, int flag);
extern void func_001764E0(char *p);
extern void func_00179560(char *p);
extern void func_001796C0(char *p);
extern int func_001821E0(unsigned char *e, int a1, int a2);
extern int func_00188610(unsigned char *e);
extern float func_001B1470(float);
extern float D_00248670[];
extern float D_700038A0[];
extern int *D_00275B40;

void func_0016EF50(unsigned char *arg0) {
    unsigned char st;
    float dur;
    int kind;

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
            *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0);
            *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8);
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
            func_001751A0(arg0);
            arg0[0x25C] = arg0[0x23F];
            kind = *(int *)(arg0 + 0x24C);
            if (kind == 0) {
                *(int *)0x700038A0 = 0;
                *(int *)0x700038A4 = 0;
                *(float *)0x700038A8 = 5.0f;
                *(float *)0x700038AC = 1.0f;
                func_001026A0(arg0 + 0xB0, arg0 + 0xD0, D_700038A0);
                *(float *)(arg0 + 0xB4) += -0.2f;
                arg0[0x25C] = 1;
                arg0[5] = 5;
                arg0[6] = 0;
                arg0[0x1F0] = 0xB;
            } else if (kind == 3) {
                if (arg0[0x23F] == 3) {
                    func_001749A0(arg0, 0x14B, 0, 1.0f);
                } else {
                    func_001749A0(arg0, 0x149, 0, 1.0f);
                }
                dur = D_00248670[arg0[0x23F]];
                *(float *)(arg0 + 0x26C) = dur;
                *(float *)(arg0 + 0x204) = dur;
                arg0[0x2F1] = 1;
                arg0[7] = arg0[7] + 1;
            } else if (kind == 2) {
                if (arg0[0x23F] == 3) {
                    func_001749A0(arg0, 0x14A, 0, 1.0f);
                } else {
                    func_001749A0(arg0, 0x148, 0, 1.0f);
                }
                dur = D_00248670[arg0[0x23F]];
                *(float *)(arg0 + 0x26C) = dur;
                *(float *)(arg0 + 0x204) = dur;
                arg0[0x2F1] = 0;
                arg0[7] = arg0[7] + 1;
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
        *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0);
        *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8);
        func_001764E0((char *)arg0);
        break;
    case 0x16:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            anim_eval_skeleton((char *)arg0);
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
            func_00174AB0((char *)arg0);
            arg0[5] = 0;
            arg0[6] = 0;
            arg0[0x1F0] = 0;
        } else {
            *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0);
            *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8);
        }
        func_001764E0((char *)arg0);
        break;
    }
    *(float *)(arg0 + 0xB4) += -0.2f;
    func_00175900(arg0, 0);
    func_001796C0((char *)arg0);
}
