// NEARMISS func_00163E90  (vram 0x00163E90, 0x38C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.96% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 2 of 227 instructions, both the SAME compare in case 3 (the func_00174AC0 arm): target `slti $at,$v0,0x2` + `bnez $at,...`; mwcc 2.3.3 emits `slti $v0,$v0,0x2` + `bnez $v0,...` (it reuses the now-dead lbu destination instead of the assembler temp). Pure register choice for a branch-feeding compar...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: Cut-scene / set-piece actor state machine driven by the state byte
// e[7], dispatched through jtbl_0026D600 (16 entries; 5..9 and 15 are empty and
// index >= 16 falls straight to the shared tail).
//   0  - advance state, fire func_001749A0(e, 0x75, 0, 1.0f), set e[0x302]=1 and
//        clear the 0x2EC block pointer.
//   1  - only while e[0x3C] <= 40.0f. If e[0x220] <= 0.0f: when e[0x234]==1 set
//        e[4]=2, e[5]=3, e[6]=0, e[0x1F0]=0x3F, otherwise jump to state 0xA with
//        e[0x1F0]=0x40. If e[0x220] > 0.0f: advance state, arm the 0x3C-tick
//        timer at e[0x20E] and clear e[0x302].
//   2  - advance when input flag 0x1000 is set in e[0x200].
//   3  - func_00174AC0(e, 0); if e[0x23F] >= 2 advance and func_0017C440(e, 0),
//        else clear e[0x25C] and func_0017C540(e).
//   4  - func_00178B90(e, 0); unless flag 0x8000 is set, func_0017C540(e).
//   10 - advance and fire func_001749A0(e, 0x1C3, 0, 8.0f).
//   11 - advance when flag 0x8000 is clear.
//   12 - only while e[0x3C] <= 38.0f: func_001B61C0(1, 0xEE, 0x3C, 1), then
//        func_001FBD50(e, e[0x234]==1 ? 0x14F : 0x14E, 0, 300.0f), then advance.
//   13 - when flag 0x1000 is set: advance, arm the 0x78-tick timer at e[0x28],
//        run anim_eval_skeleton(e), then stage a scratchpad vector at 0x700038A0
//        from the camera/target record q = *(void **)(D_00275B40 + 4)
//        (x = q[0xC0], y = 0.1f + e[0x250], z = q[0xC8], w = 1.0f) and call
//        func_001EFD90(0x80000043, D_700038A0, e + 0xB0, q).
//   14 - count the e[0x28] timer down; on the tick where it read 0, advance and
//        func_001AEDE0(4, 0).
// The shared tail always runs func_001764E0(e); if e[0x302] is set it publishes
// the 0x2EC block via func_00179880 and func_00175900(e, 1), otherwise it decays
// e[0xB4] by 0.2f, calls func_00175900(e, 1) and func_001796C0(e).
extern int anim_eval_skeleton(void *e);
extern int func_001749A0(void *e, int id, int flag, float v);
extern int func_00174AC0(void *e, int a);
extern int func_00175900(void *e, int a);
extern int func_001764E0(void *e);
extern int func_00178B90(void *e, int a);
extern int func_001796C0(void *e);
extern int func_00179880(void *e, void *p);
extern int func_0017C440(void *e, int a);
extern int func_0017C540(void *e);
extern int func_001AEDE0(int a, int b);
extern int func_001B61C0(int a, int b, int c, int d);
extern int func_001EFD90(int a, void *b, void *c, void *d);
extern int func_001FBD50(void *e, int id, int flag, float v);
extern char *D_00275B40;
extern int D_700038A0[4];

void func_00163E90(char *e) {
    unsigned char st = *(unsigned char *)(e + 7);
    char *q;
    char *g;
    short tmr;

    switch (st) {
    case 0:
        (*(unsigned char *)(e + 7))++;
        func_001749A0(e, 0x75, 0, 1.0f);
        *(char *)(e + 0x302) = 1;
        *(int *)(e + 0x2EC) = 0;
        break;
    case 1:
        if (*(float *)(e + 0x3C) <= 40.0f) {
            if (*(float *)(e + 0x220) <= 0.0f) {
                if (*(unsigned char *)(e + 0x234) == 1) {
                    *(char *)(e + 4) = 2;
                    *(char *)(e + 5) = 3;
                    *(char *)(e + 6) = 0;
                    *(char *)(e + 0x1F0) = 0x3F;
                } else {
                    *(char *)(e + 7) = 0xA;
                    *(char *)(e + 0x1F0) = 0x40;
                }
            } else {
                *(char *)(e + 7) = st + 1;
                *(short *)(e + 0x20E) = 0x3C;
                *(char *)(e + 0x302) = 0;
            }
        }
        break;
    case 2:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(char *)(e + 7) = st + 1;
        }
        break;
    case 3:
        func_00174AC0(e, 0);
        if (!(*(unsigned char *)(e + 0x23F) < 2)) {
            *(unsigned char *)(e + 7) = *(unsigned char *)(e + 7) + 1;
            func_0017C440(e, 0);
        } else {
            *(char *)(e + 0x25C) = 0;
            func_0017C540(e);
        }
        break;
    case 4:
        func_00178B90(e, 0);
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            func_0017C540(e);
        }
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    case 8:
        break;
    case 9:
        break;
    case 10:
        *(char *)(e + 7) = st + 1;
        func_001749A0(e, 0x1C3, 0, 8.0f);
        break;
    case 11:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(char *)(e + 7) = st + 1;
        }
        break;
    case 12:
        if (*(float *)(e + 0x3C) <= 38.0f) {
            func_001B61C0(1, 0xEE, 0x3C, 1);
            if (*(unsigned char *)(e + 0x234) == 1) {
                func_001FBD50(e, 0x14F, 0, 300.0f);
            } else {
                func_001FBD50(e, 0x14E, 0, 300.0f);
            }
            *(unsigned char *)(e + 7) = *(unsigned char *)(e + 7) + 1;
        }
        break;
    case 13:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(char *)(e + 7) = st + 1;
            *(short *)(e + 0x28) = 0x78;
            anim_eval_skeleton(e);
            g = D_00275B40;
            q = *(char **)(g + 4);
            *(float *)0x700038A0 = *(float *)(q + 0xC0);
            *(float *)0x700038A8 = *(float *)(*(char **)(g + 4) + 0xC8);
            *(float *)0x700038A4 = 0.1f + *(float *)(e + 0x250);
            *(int *)0x700038AC = 0x3F800000;
            func_001EFD90(0x80000043, D_700038A0, e + 0xB0, q);
        }
        break;
    case 14:
        tmr = *(short *)(e + 0x28);
        *(short *)(e + 0x28) = tmr - 1;
        if (tmr == 0) {
            *(unsigned char *)(e + 7) = *(unsigned char *)(e + 7) + 1;
            func_001AEDE0(4, 0);
        }
        break;
    case 15:
        break;
    default:
        break;
    }
    func_001764E0(e);
    if (*(unsigned char *)(e + 0x302) != 0) {
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        return;
    }
    *(float *)(e + 0xB4) += -0.2f;
    func_00175900(e, 1);
    func_001796C0(e);
}
