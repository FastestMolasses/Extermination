// NEARMISS func_001ACEC0  (vram 0x001ACEC0, 0x148 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Delay-slot-fill scheduling wall: in the case-0 arm mwcc speculatively hoists `lui at, 0x7000` (CSE'd slot-pointer address build) into the `bnez D_00275BE0` delay slot, where the target (CW) leaves a nop and rebuilds the lui per-arm. Switch idiom already unlocked the dispatch+branch-inversion (88....
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p

//
// The GAME TASK installed in frame-task slot 0. On entry stores 2 to scratchpad
// byte 0x70003B90, then dispatches on the task slot's state byte +8 (slot ptr
// in spad 0x70003B6C):
//   0: if func_001AD1A0() done: +8 = (gp-rel D_00275BE0 == 0) ? 1 : 2,
//      clear +9/+0xA; then func_001D1EF0() unconditionally.
//   1: if func_001AD230(): +8 = 3, clear +9/+0xA/+0xB.
//   2: +8 = 3 (via the slot+8 pointer computed at entry), +9 = 5, clear +0xA/+0xB.
//   3: func_001AD250() (the sub-state machine).
//
// Logic fully recovered. Writing the dispatch as a `switch` (mwcc reverses to
// the target's 3,2,1,0 descending compare order) cleared the prior 88.8% branch-
// inversion/dead-b wall and reached 97.5%. SOLE RESIDUAL: in the case-0 arm,
// mwcc fills the `bnez D_00275BE0` delay slot with a speculatively-hoisted
// `lui at, 0x7000` (the slot-pointer address build it CSEs into both arms),
// whereas the target (CW) leaves the delay slot a nop and rebuilds the lui in
// each arm. This is the documented delay-slot-fill scheduling wall -- 2.3.3 does
// not change it; not the clean-store nop case. Tried: nv-temp (mwcc turned the
// two-arm store into a movn, worse), inverted condition (flipped values, worse),
// -O3/-O2/-inline off (no change). Parked as a scheduling/delay-slot near-miss.
extern int func_001AD1A0(void);
extern int func_001AD230(void);
extern void func_001AD250(void);
extern void func_001D1EF0(void);
extern unsigned char D_00275BE0;
void func_001ACEC0(void) {
    char *slot;
    unsigned char *statep;
    slot = *(char * volatile *)0x70003B6C;
    statep = (unsigned char *)(slot + 8);
    *(volatile unsigned char *)0x70003B90 = 2;
    switch (*(unsigned char *)(slot + 8)) {
    case 0:
        if (func_001AD1A0()) {
            if (D_00275BE0 == 0)
                *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x8) = 1;
            else
                *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x8) = 2;
            *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x9) = 0;
            *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xA) = 0;
        }
        func_001D1EF0();
        break;
    case 1:
        if (func_001AD230()) {
            *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x8) = 3;
            *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x9) = 0;
            *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xA) = 0;
            *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xB) = 0;
        }
        break;
    case 2:
        *statep = 3;
        *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x9) = 5;
        *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xA) = 0;
        *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xB) = 0;
        break;
    case 3:
        func_001AD250();
        break;
    }
}
