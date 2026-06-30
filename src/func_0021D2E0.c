// NEARMISS func_0021D2E0  (vram 0x0021D2E0, 0x1A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP operand-coloring on a single commutative add.s. Body + structure 100% recovered and byte-identical EXCEPT one instruction at 0x168: target emits `add.s $f0,$f1,$f0` (field-operand first) for `*(arg0+0xB4) + -0.2f`, mwcc 2.3.3 emits `add.s $f0,$f0,$f1` (const-operand first). Tried temp-hoist an...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 99.90% (mwcc 2.3.3; pinned 991202 = 93.0%). Body + structure fully
// recovered and byte-identical except ONE commutative add.s operand swap at the
// arg0+0xB4 fade-add: target `add.s f0,f1,f0` (field first) vs mwcc `add.s f0,f0,f1`
// (const first). FP operand-coloring artifact mwcc won't flip via source -> permuter.
//
// Per-state driver dispatched on the state byte at arg0+7 (switch 0/1/2):
//  state 0: bump state, set arg0+0=2, arg0+0x28=arg1, zero arg0+0x220; if the
//   gate byte arg0+0x25F==0 and the flag word arg0+0x300 & 0x8000 is clear, run
//   anim_eval_skeleton(arg0), then seed the scratch vec4 at 0x700038A0 from the
//   bone block *(D_00275B40+4) (+0xC0 / +0xC8 floats) plus 0.1f+*(arg0+0x250),
//   w=1.0f, and dispatch func_001EFD90(0x80000043, &scratch, arg0+0xB0, bone).
//  state 1: if anim id arg0+0x1F0==0xE && arg0+0x319!=0 fire func_001749A0(self,
//   0x2B,0,1.0f); decrement the timer arg0+0x28 and on underflow bump the state
//   and, unless arg0+0xF==0xB, run func_001AEDE0(4,0).
//  state 2: no-op.
// Always (when arg2==0): fade arg0+0xB4 by -0.2f, func_00179880(self, self+0x2EC),
//  and if func_00175900(self,1) zero the int at arg0+0x2EC.
extern void anim_eval_skeleton(char *);
extern void func_001749A0(char *, int, int, float);
extern int func_00175900(char *, int);
extern void func_00179880(char *, char *);
extern void func_001AEDE0(int, int);
extern void func_001EFD90(int, void *, char *, char *);
extern char *D_00275B40;
extern char D_700038A0[];

void func_0021D2E0(char *arg0, short arg1, int arg2) {
    unsigned char st;
    short t;
    char *p;
    char *base;

    st = *(unsigned char *)(arg0 + 7);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 7) = st + 1;
        *(char *)(arg0 + 0) = 2;
        *(short *)(arg0 + 0x28) = arg1;
        *(int *)(arg0 + 0x220) = 0;
        if (*(unsigned char *)(arg0 + 0x25F) == 0 && !(*(short *)(arg0 + 0x300) & 0x8000)) {
            anim_eval_skeleton(arg0);
            base = D_00275B40;
            p = *(char **)(base + 4);
            *(float *)0x700038A0 = *(float *)(p + 0xC0);
            *(float *)0x700038A8 = *(float *)(*(char **)(base + 4) + 0xC8);
            *(float *)0x700038A4 = 0.1f + *(float *)(arg0 + 0x250);
            *(int *)0x700038AC = 0x3F800000;
            func_001EFD90(0x80000043, D_700038A0, arg0 + 0xB0, p);
        }
        break;
    case 1:
        if (*(unsigned char *)(arg0 + 0x1F0) == 0xE && *(unsigned char *)(arg0 + 0x319) != 0) {
            func_001749A0(arg0, 0x2B, 0, 1.0f);
        }
        t = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = t - 1;
        if (t == 0) {
            *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) + 1;
            if (*(unsigned char *)(arg0 + 0xF) != 0xB) {
                func_001AEDE0(4, 0);
            }
        }
        break;
    case 2:
        break;
    }
    if (arg2 == 0) {
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + -0.2f;
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            *(int *)(arg0 + 0x2EC) = 0;
        }
    }
}
