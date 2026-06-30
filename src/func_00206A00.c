// NEARMISS func_00206A00  (vram 0x00206A00, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.66% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling permutation: target sinks a0->s5 save into the first call's delay slot and orders the idx=5 call's a2(fnptr)-before-a1(=5) arg setup; mwcc schedules both the other way. All logic, calls, args and frame match. Permuter-class.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 98.66% (mwcc 2.3.3; pinned 991202 reaches 83.0%). Body, all calls,
// argument values, the 8-in/6-forwarded parameter shape, and frame are verified
// correct. Two residual deltas, both pure instruction-scheduling permutations:
// (1) the target sinks the a0->$s5 save into the delay slot of the first (argless)
// call, mwcc emits it among the other arg saves; (2) for the idx==5 register()
// call the target finishes the function-pointer arg ($a2 = lui+addiu) before
// loading $a1=5, mwcc loads $a1=5 first. Both are list-scheduler choices, not
// logic. Permuter territory.
//
// Semantics: a subsystem/state-machine registration. Takes 8 args; a1/a2 are
// unused. Emits a work-area-size diagnostic (sub_The_size_of_work_area_is_too_sma),
// then registers five state handlers on object a0 via func_00109AF8 at indices
// 0,1,2,3,5 (-> func_00207060/70/A0/D0/100). Runs func_00206B20(a0) init, then
// forwards the tail args to func_002041D0(a0+0x48, a3, t0, t1, t2, t3). Returns 1.

extern void sub_The_size_of_work_area_is_too_sma(void);
extern void func_00109AF8(int a0, int idx, void *fn, int d);
extern void func_00206B20(int a0);
extern void func_002041D0(int a0, int a1, int a2, int a3, int t0, int t1);
extern void func_00207060(void);
extern void func_00207070(void);
extern void func_002070A0(void);
extern void func_002070D0(void);
extern void func_00207100(void);

int func_00206A00(int a0, int a1, int a2, int a3, int t0, int t1, int t2, int t3) {
    sub_The_size_of_work_area_is_too_sma();
    func_00109AF8(a0, 0, func_00207060, 0);
    func_00109AF8(a0, 1, func_00207070, 0);
    func_00109AF8(a0, 2, func_002070A0, 0);
    func_00109AF8(a0, 3, func_002070D0, 0);
    func_00109AF8(a0, 5, func_00207100, 0);
    func_00206B20(a0);
    func_002041D0(a0 + 0x48, a3, t0, t1, t2, t3);
    return 1;
}
