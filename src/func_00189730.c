// NEARMISS func_00189730  (vram 0x00189730, 0x290 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.16% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 8 of ~166 instrs differ, in two independent spots inside case 3 (everything else, including the jr-table dispatch and all other 6 cases, is byte-identical). (1) 0x13c/0x140: target `bnez v1,.L00189880; nop`; mwcc speculates the taken path's first op `lui at,0x7000` (scratchpad 0x70003B68 address-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 98.16% (mwcc 2.3.3). Body verified instruction-for-instruction
// against the target; two residuals, both compiler artifacts:
//   * case 3: mwcc speculates the taken path's `lui at,0x7000` (scratchpad
//     address-hi) into the `bnez` delay slot where the target keeps a nop
//     (idiom-13 delay-slot fill; not cured by 2.3.3 nor by `volatile`).
//   * case 3 table lookup: register-allocation permutation. The target keeps
//     &D_0024A410 in $a2 (copy_qw4's arg-3 register) and reuses it as the index
//     base, putting the arg-2 `addiu ...,0x90` in the jal delay slot; mwcc
//     colours the base $a1 and therefore re-materializes lui/addiu %hi/%lo for
//     arg 3. Same instruction order, +1 lui/addiu pair.
//
// SEMANTICS: 7-state scripted-camera/anim state machine. p is the script state
// block (p[5] = state, p+0x28 = s16 timer/step counter, p+0x2A = s16 variant
// selector, p+0x38 = float per-frame delta), e is the actor/entity block
// (e[4] = active flag, e[0x1F0] = action id, e[0x275] = sub-action,
// e+0x20C = s16 anim id, e+0x3C = float distance). The state machine ramps the
// float at offset 0x74 of *(D_00275B40+4) (a camera/view field) down and back
// up, and in state 3 streams matrices out of D_0024A410/D_0024A3B0 into the
// scratchpad buffers D_700036A0/D_700036D0. Dropping e[4] out of state 1 resets
// the machine to state 0.

extern short D_00248B9C[8];
extern short D_00248C7C[8];
extern int D_0024A3B0[];
extern char D_0024A410[];
extern unsigned char *D_00275B40;
extern int D_700036A0[];
extern int D_700036D0[];

extern int copy_qw4(int *, int, char *, int);
extern int func_001026A0(int *, int, int *);
extern int func_00122BB8(void);
extern int func_001F4010(int, int *);

void func_00189730(unsigned char *p, unsigned char *e) {
    int st;
    int n;
    int k;
    char *tbl;

    if (e[4] != 1) {
        p[5] = 0;
        *(int *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) = 0;
    }
    st = p[5];
    switch (st) {
    case 0:
        if (e[0x275] == 2 && e[0x1F0] == 0x33) {
            p[5]++;
        }
        break;
    case 1:
        if (*(short *)(e + 0x20C) == D_00248B9C[0] || *(short *)(e + 0x20C) == D_00248C7C[0]) {
            p[5]++;
            *(short *)(p + 0x28) = 8;
            *(float *)(p + 0x38) = -0.07875798f;
        }
        break;
    case 2:
        n = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = n - 1;
        if (n == 0) {
            p[5] = p[5] + 1;
            *(float *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) = -0.63006383f;
            *(short *)(p + 0x28) = 0;
            *(short *)(p + 0x2A) = func_00122BB8() & 1;
        } else {
            *(float *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) += *(float *)(p + 0x38);
        }
        break;
    case 3:
        if (*(short *)(p + 0x28) >= 6) {
            p[5] = st + 1;
        } else if ((*(volatile int *)0x70003B68 & 1) == 0) {
            *(short *)(p + 0x28) = *(short *)(p + 0x28) + 1;
            tbl = D_0024A410;
            k = *(short *)(p + 0x2A) * 3;
            n = *(int *)(*(short *)(p + 0x28) * 4 + &tbl[k * 8]);
            copy_qw4(D_700036A0, *(int *)(D_00275B40 + 8) + 0x90, tbl, k);
            func_001026A0(D_700036D0, *(int *)(D_00275B40 + 8) + 0x90, &D_0024A3B0[n * 4]);
            func_001F4010(8, D_700036A0);
        }
        break;
    case 4:
        if (*(float *)(e + 0x3C) <= 11.0f) {
            p[5] = st + 1;
            *(short *)(p + 0x28) = 8;
        }
        break;
    case 5:
        n = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = n - 1;
        if (n == 0) {
            p[5] = p[5] + 1;
            *(int *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) = 0;
        } else {
            *(float *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) -= *(float *)(p + 0x38);
        }
        break;
    case 6:
        if (e[0x1F0] != 0x33) {
            p[5] = 0;
        }
        break;
    }
}
