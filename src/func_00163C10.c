// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame state-machine tick for an actor (arg0). Dispatches on the
// 1-byte state field arg0[0x7]:
//   state 0: advance state (++), kick off anim/clip 0x6E at blend 1.0
//            (func_001749A0).
//   state 1: if the 0x200 flags word has bit 0x1000 set, advance state.
//   state 2: func_00174AC0(arg0,0); if sub-state byte arg0[0x23F] > 1 then
//            advance state and func_0017C440(arg0,0); else clear arg0[0x25C]
//            and func_0017C540(arg0).
//   state 3: func_00178B90(arg0,0); unless 0x200 flags bit 0x8000 set,
//            func_0017C540(arg0).
// Common tail (all states): func_001764E0(arg0); nudge float arg0[0xB4] by
// -0.2; func_00175900(arg0,1); func_001796C0(arg0).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the residual was wall #13 (clean-store delay-slot fill) plus an
// slti/sltiu lowering difference; the 2.3.3 build is byte-identical (objdiff
// 100% vs build/expected/func_00163C10.o). NOTE: the state-2 sub-state test
// is written `arg0[0x23F] > 1` (not `>= 2`): only `> 1` makes mwcc lower it to
// `slti $at,v0,2; bnez $at`, matching the target's $at allocation.
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_00174AC0(unsigned char *e, int f);
extern void func_0017C440(unsigned char *e, int f);
extern void func_0017C540(unsigned char *e);
extern void func_00178B90(unsigned char *e, int f);
extern void func_001764E0(unsigned char *e);
extern void func_00175900(unsigned char *e, int f);
extern void func_001796C0(unsigned char *e);

void func_00163C10(unsigned char *arg0) {
    switch (arg0[0x7]) {
    case 0:
        arg0[0x7] = arg0[0x7] + 1;
        func_001749A0(arg0, 0x6E, 0, 1.0f);
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[0x7] = arg0[0x7] + 1;
        }
        break;
    case 2:
        func_00174AC0(arg0, 0);
        if (arg0[0x23F] > 1) {
            arg0[0x7] = arg0[0x7] + 1;
            func_0017C440(arg0, 0);
        } else {
            arg0[0x25C] = 0;
            func_0017C540(arg0);
        }
        break;
    case 3:
        func_00178B90(arg0, 0);
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            func_0017C540(arg0);
        }
        break;
    }

    func_001764E0(arg0);
    *(float *)(arg0 + 0xB4) += -0.2f;
    func_00175900(arg0, 1);
    func_001796C0(arg0);
}
