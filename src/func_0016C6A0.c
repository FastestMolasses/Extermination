// NEARMISS func_0016C6A0  (vram 0x0016C6A0, 0x6C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.70% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// anim_clip_arbiter dead-float-argument scheduling + func_00224290 branch-likely tail scheduling; logic 100% recovered.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Player crouch/land vertical-motion state machine, dispatched on the state
// byte at arg0+6 (a beq-chain, not a jump table: compare order 0x1E,0x15,
// 0x14,0xC,0xB,0xA,3,2,1,0, default-fallthrough). arg0 is the player actor.
//   state 0: enter crouch — sfx/particle setup (func_0016C570), lower profile
//            -0.6, animate 0x5E, arm blend timers, sound 0x12E.
//   state 1/2: turn-lean blend toward +0x218 (func_001B12B0 step 6deg/frame);
//            func_00224B80==2 aborts to func_0016C520 (cleanup).
//   state 3: forwards to func_0017F5F0/func_0016CD70 (footstep/turn driver),
//            aborting the same way on func_00224B80==2.
//   state 0xA: stand back up — restore +0xC4 from +0x218, clip via
//            func_001749A0, clear crouch trims, sound cue func_00182870(1).
//   state 0xB/0xC: airborne/fall physics + footstep-timer sub-state machine
//            (func_0016CD70(func_0016C6A0 vram) is the case 0xC arm from the
//            crouch-land FINDINGS note); gated on the 0x1000 "grounded" bit at
//            +0x200.
//   state 0x14: land — animate clip 0x73 (LANDING), arm settle timers.
//   state 0x15: land settle — ease +0xC0 toward 0.0f (func_001B12B0, step
//            4deg/frame), consume the airborne timer against the fall-rate
//            +0x2E0, and dispatch to func_0017C580 or a func_001749A0(0x6D)
//            recover clip depending on func_00224290(arg0).
//   state 0x1E: func_0021D2E0(arg0, 0x78, 0) — external abort/teleport hook.
// Tail: unless state==0x1E, if the surface id at +0x23A==0x5D, fire the
// splash/impact hook func_0021D250(arg0, 0).
extern void anim_clip_arbiter(unsigned char *, int, int, float);
extern float func_0011DF78(float);
extern void func_0016C520(unsigned char *);
extern void func_0016C570(unsigned char *);
extern int func_0016CD70(unsigned char *, int);
extern int func_001749A0(unsigned char *, int, int, float);
extern int func_00175900(unsigned char *, int);
extern void func_00178B90(unsigned char *, int);
extern int func_001791D0(unsigned char *, int);
extern void func_001796C0(unsigned char *);
extern void func_00179880(unsigned char *, float *);
extern void func_0017C580(unsigned char *);
extern int func_0017F5F0(unsigned char *, int);
extern void func_00182430(unsigned char *, int);
extern void func_00182870(unsigned char *, int);
extern float func_001B12B0(float, float, float);
extern int func_001C61D0(int, int);
extern int func_001FBD50(unsigned char *, int, int, float);
extern void func_0021D250(unsigned char *, int);
extern void func_0021D2E0(unsigned char *, int, int);
extern int func_00224290(unsigned char *);
extern int func_00224B80(unsigned char *);
extern char **D_00275B40;

void func_0016C6A0(unsigned char *arg0)
{
    unsigned char *e = arg0;
    float z = 0.0f;

    switch (e[6]) {
    case 0:
        func_0016C570(e);
        *(float *)(e + 0xB4) += -0.6f;
        func_00175900(e, 0);
        e[6] = e[6] + 1;
        e[7] = 0;
        e[0x25D] = 0;
        *(float *)(e + 0x38) = func_0011DF78(-0.2f);
        *(float *)(e + 0x2EC) = z;
        *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(e + 0x40), 0x5E);
        anim_clip_arbiter(e, 0x5E, 8.0f, *(float *)0x70003A20 - 15.0f);
        e[0x1F1] = 0;
        *(int *)(e + 0x2E4) = 0;
        *(float *)(e + 0x294) = *(float *)(e + 0xB4);
        *(short *)(e + 0x2A) = 0;
        *(int *)(e + 0x2F8) = 0;
        e[0x31B] = (unsigned char)func_001FBD50(e, 0x12E, 0, 300.0f);
        e[0x31A] = 1;
        *(short *)(e + 0x31C) = 0x12E;
        break;
    case 1:
        if (func_00224B80(e) == 2) {
            func_0016C520(e);
        } else {
            *(float *)(e + 0xC4) = func_001B12B0(*(float *)(e + 0x218), *(float *)(e + 0xC4), 0.10471976f);
            *(float *)(e + 0xC0) = func_001B12B0(*(float *)(e + 0x9C), *(float *)(e + 0xC0), 0.10471976f);
            if (*(float *)(e + 0xC0) == *(float *)(e + 0x9C)) {
                e[6] = e[6] + 1;
                *(float *)(e + 0x2EC) = z;
            }
        }
        func_001791D0(e, 0);
        break;
    case 2:
        if (func_00224B80(e) == 2) {
            func_0016C520(e);
        } else {
            *(float *)(e + 0xC4) = func_001B12B0(*(float *)(e + 0x218), *(float *)(e + 0xC4), 0.10471976f);
            if (*(float *)(e + 0xC4) == *(float *)(e + 0x218)) {
                e[6] = e[6] + 1;
            }
        }
        func_001791D0(e, 0);
        break;
    case 3: {
        int r = func_00224B80(e);
        if (r == 2) {
            func_0016C520(e);
        } else {
            func_0017F5F0(e, r);
            func_0016CD70(e, r);
        }
        break;
    }
    case 0xA:
        *(float *)(e + 0xC4) = *(float *)(e + 0x218);
        e[6] = e[6] + 1;
        *(float *)(e + 0xC0) = z;
        func_001749A0(e, 0x60, 0, 1.0f);
        e[0x25F] = 0;
        *(float *)(e + 0x21C) = z;
        *(float *)(e + 0x38) = z;
        *(float *)(e + 0x2EC) = z;
        func_00182870(e, 1);
        break;
    case 0xB:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = e[6] + 1;
            e[0x302] = 0;
            func_001749A0(e, 0x65, 0, 1.0f);
            *(float *)(e + 0x21C) = z;
            *(float *)(e + 0x38) = z;
        } else {
            *(float *)(e + 0x38) = *(float *)((char *)*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)((char *)*D_00275B40 + 8);
            func_00178B90((unsigned char *)1, 0);
        }
        *(float *)(e + 0xB4) += -0.6f;
        *(float *)(e + 0xB4) += *(float *)(e + 0x2EC);
        func_00175900(e, 1);
        func_001796C0(e);
        break;
    case 0xC:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[5] = 0;
            e[6] = 0;
            e[0x1F0] = 0;
        } else {
            *(float *)(e + 0x38) = *(float *)((char *)*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)((char *)*D_00275B40 + 8);
            *(float *)(e + 0x38) *= *(float *)(e + 0x26C);
            func_00178B90((unsigned char *)1, 0);
            switch (e[0x302]) {
            case 0:
                if (*(float *)(e + 0x3C) <= 24.0f) {
                    e[0x302] = e[0x302] + 1;
                    func_00182430(e, 2);
                }
                break;
            case 1:
                if (*(float *)(e + 0x3C) <= 13.0f) {
                    e[0x302] = e[0x302] + 1;
                    func_00182430(e, 2);
                }
                break;
            case 2:
                if (*(float *)(e + 0x3C) <= 2.0f) {
                    e[0x302] = e[0x302] + 1;
                    func_00182430(e, 2);
                }
                break;
            }
        }
        *(float *)(e + 0xB4) += -0.4f;
        func_00175900(e, 1);
        func_001796C0(e);
        break;
    case 0x14:
        e[6] = e[6] + 1;
        *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(e + 0x40), 0x73);
        anim_clip_arbiter(e, 0x73, 8.0f, *(float *)0x70003A20 - 10.0f);
        *(float *)(e + 0x2F4) = *(float *)(e + 0xB4);
        *(float *)(e + 0x2EC) = z;
        *(float *)(e + 0x2E0) = *(float *)(e + 0x38) / 60.0f;
        break;
    case 0x15: {
        int landed;
        *(float *)(e + 0xC0) = func_001B12B0(0.0f, *(float *)(e + 0xC0), 0.06981317f);
        if (*(float *)(e + 0x38) <= *(float *)(e + 0x2E0)) {
            *(float *)(e + 0x38) = z;
        } else {
            *(float *)(e + 0x38) -= *(float *)(e + 0x2E0);
        }
        landed = func_00224290(e);
        func_00178B90(e, 1);
        if (e[0x314] & 1) {
            *(float *)(e + 0x38) = z;
            e[5] = 7;
            e[6] = 0;
            e[0x1F0] = 0xD;
        } else {
            func_00179880(e, (float *)(e + 0x2EC));
            func_00175900(e, 1);
            if (e[0xA] != 0) {
                if (landed == 0) {
                    func_0017C580(e);
                } else {
                    func_001749A0(e, 0x6D, 0, 1.0f);
                }
            }
        }
        break;
    }
    case 0x1E:
        func_0021D2E0(e, 0x78, 0);
        break;
    }

    if (e[6] != 0x1E && e[0x23A] == 0x5D) {
        func_0021D250(e, 0);
    }
}
