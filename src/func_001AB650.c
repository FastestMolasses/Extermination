// CFLAGS: -O4,p -sdatathreshold 0
// Frame-task table zero: memset(slot, 0, 0x20) over the 3 slots at D_0028A750
// (func_00121A28 is the engine memset). Called once from the main loop's init
// block (func_001AAE40) before task_register(0, boot task 0x001AB7E0).
//
// DECOMP WIP — NOT byte-matching (89.5%, up from 68.7%); linked from the
// splat .s via fill_unmatched SIZE_DRIFT_FORCE_ASM. Two residual diffs, both
// documented compiler walls (docs/PROGRESS.md):
//   1. slti-into-branch regalloc: CW emits `slti $v1, $s0, 3; bnez $v1`;
//      mwcc always sinks the compare into $at (`more = i < 3` as an explicit
//      local does not help — falsified again here).
//   2. Prologue interleave: CW schedules `lui $v0` between `sq $s1` and
//      `sq $s0`; mwcc emits all register saves contiguously.
// The call-arg ordering (a2=0x20 first, a1=0 in the jal delay slot) WAS
// recovered with the statement-order idiom: `n = 0x20;` as its own statement
// before the call.
extern void func_00121A28(char *, int, int);
extern char D_0028A750;

void func_001AB650(void) {
    int i;
    char *p;
    int n;
    int more;

    p = &D_0028A750;
    i = 0;
top:
    n = 0x20;
    func_00121A28(p, 0, n);
    i++;
    more = i < 3;
    p += 0x20;
    if (more) goto top;
}
