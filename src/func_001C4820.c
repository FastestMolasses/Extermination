// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS (2026-06-10 s15): generic placed-prop / item-pickup behavior —
// the placement-record +0x24 target for every kind-0xB pickup. State machine
// on actor byte +0x04: 0=INIT (func_001B0FD0 binds model/bones, then
// func_001C6380 stamps the placement TRS into all bone slots), 1=ACTIVE
// (func_001B17A0 common prop frame, then the anim-mode method at +0x4C),
// 2/3=FREE (func_001AFC10).
//
// Matched 100.0 with mwcc 2.3.3 (s84): the idiom-13 clean-store delay-slot
// nop wall — the pinned 991202 build hoists func_001C6380's paddub a0,s0
// into the bnez delay slot and stalls at 89.74%; 2.3.3 leaves
// `bnez v0,skip; nop` and puts paddub in the jal delay slot, exactly as the
// target. The other fix was single-argument callee signatures (the original
// 4-arg decls forced spurious a1/a2/a3=0 arg materialization everywhere).
extern void func_001AFC10(int);
extern int func_001B0FD0(int);
extern void func_001B17A0(int);
extern void func_001C6380(int);

void func_001C4820(int a0) {
    switch (*(unsigned char *)(a0 + 0x4)) {
    case 0:
        if (func_001B0FD0(a0) == 0) {
            func_001C6380(a0);
        }
        break;
    case 1:
        func_001B17A0(a0);
        (*(void (**)(int))(a0 + 0x4C))(a0);
        break;
    case 2:
    case 3:
        func_001AFC10(a0);
        break;
    }
}
