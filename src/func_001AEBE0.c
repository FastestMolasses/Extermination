// NEARMISS func_001AEBE0  (vram 0x001AEBE0, 0x1A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.47% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation. Body + structure fully recovered and byte-identical through offset 0x144; the SOLE residual (11 instrs, all in the final if-block) is an a2<->a3 register-coloring swap: target colors dl(D_00275670)->a2 and block ptr blk->a3, mwcc 2.3.3 picks the opposite. Id...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// HUD/display-list state driver. Dispatches on the state short D_0028A8D0
// (cases 0/1: idle; 2: ramp the level D_0028A8D4 down by D_0028A8D2, clamp at 0
// and reset state; 3: ramp up by D_0028A8D2, clamp at 0xFF and set state 1).
// Then writes the current level into three 0x30-byte display-list block fields
// (+0x30/+0x34/+0x38) for the 2 sub-blocks at D_0028A7B0 + D_00810E80*0x90.
// Finally, unless the scratchpad gate at 0x70003B90==2 && D_008106C4 says skip,
// and only if the ORIGINAL state was non-zero, appends a GIF/DMA tag into the
// display-list cursor held at D_00275670+0x14 (gp-rel pointer global): byte+3 =
// 0x30, word+4 = block ptr, half+0 = 9, advance cursor by 0x10.
//
// NEARMISS 99.47% via mwcc 2.3.3; the pinned 991202 build reaches 89.71%. Body
// byte-identical through 0x144. Sole residual: an a2<->a3 register-coloring swap
// in the final if-block (dl<->blk) -- register-allocation-order permuter.
extern char *D_00275670;
extern char D_0028A7B0[];
extern short D_0028A8D0[16];
extern short D_0028A8D2[16];
extern int D_0028A8D4[16];
extern unsigned char D_008106C4[16];
extern short D_00810E80[16];

void func_001AEBE0(void) {
    int st;
    int i;
    char *p;
    char *dl;

    st = D_0028A8D0[0];
    switch (st) {
    case 0:
    case 1:
        break;
    case 2:
        D_0028A8D4[0] -= D_0028A8D2[0];
        if (D_0028A8D4[0] < 0) {
            D_0028A8D4[0] = 0;
            D_0028A8D0[0] = 0;
        }
        break;
    case 3:
        D_0028A8D4[0] += D_0028A8D2[0];
        if (D_0028A8D4[0] >= 0xFF) {
            D_0028A8D4[0] = 0xFF;
            D_0028A8D0[0] = 1;
        }
        break;
    }
    p = D_0028A7B0 + (D_00810E80[0] * 0x90);
    i = 0;
    do {
        i += 1;
        *(int *)(p + 0x30) = D_0028A8D4[0];
        *(int *)(p + 0x34) = D_0028A8D4[0];
        *(int *)(p + 0x38) = D_0028A8D4[0];
        p += 0x30;
    } while (i < 2);
    if (((*(unsigned char *)0x70003B90 != 2) || (D_008106C4[0] == 0)) && (st != 0)) {
        char *blk = D_0028A7B0 + (D_00810E80[0] * 0x90);
        dl = D_00275670;
        *(char *)(*(int *)(dl + 0x14) + 3) = 0x30;
        *(int *)(*(int *)(dl + 0x14) + 4) = (int)blk;
        *(short *)(*(int *)(dl + 0x14) + 0) = 9;
        *(int *)(dl + 0x14) = *(int *)(dl + 0x14) + 0x10;
    }
}
