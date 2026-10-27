// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.66% NEARMISS).
// Fixes read from the original instructions:
// - The work-area initialiser is called with this function's first three
//   arguments (a0 = context, a1 = work buffer, a2 = buffer size), which are
//   live and unchanged at the call; declaring it with its real prototype from
//   src/sub_The_size_of_work_area_is_too_sma.c lets the a0 copy into a saved
//   register fill the call's delay slot. a1/a2 are therefore not unused.
// - Only the idx==5 registration spells its handler with an explicit
//   (void *) cast; that cast is what orders the handler-address setup before
//   the index constant, as in the original.
//
// Semantics: a subsystem/state-machine registration. Initialises the work area
// for context a0 from buffer a1 of size a2 (sub_The_size_of_work_area_is_too_sma,
// which prints the "work area is too small" diagnostic on failure; its result is
// ignored), then registers five state handlers on object a0 via func_00109AF8 at
// indices 0,1,2,3,5 (-> func_00207060/70/A0/D0/100). Runs func_00206B20(a0) init,
// then forwards the tail args to func_002041D0(a0+0x48, a3, t0, t1, t2, t3).
// Returns 1.

extern int sub_The_size_of_work_area_is_too_sma(void *ctx, unsigned int buf, unsigned int size);
extern void func_00109AF8(int a0, int idx, void *fn, int d);
extern void func_00206B20(int a0);
extern void func_002041D0(int a0, int a1, int a2, int a3, int t0, int t1);
extern void func_00207060(void);
extern void func_00207070(void);
extern void func_002070A0(void);
extern void func_002070D0(void);
extern void func_00207100(void);

int func_00206A00(int a0, int a1, int a2, int a3, int t0, int t1, int t2, int t3) {
    sub_The_size_of_work_area_is_too_sma((void *)a0, a1, a2);
    func_00109AF8(a0, 0, func_00207060, 0);
    func_00109AF8(a0, 1, func_00207070, 0);
    func_00109AF8(a0, 2, func_002070A0, 0);
    func_00109AF8(a0, 3, func_002070D0, 0);
    func_00109AF8(a0, 5, (void *)func_00207100, 0);
    func_00206B20(a0);
    func_002041D0(a0 + 0x48, a3, t0, t1, t2, t3);
    return 1;
}
