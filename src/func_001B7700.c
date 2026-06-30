// NEARMISS func_001B7700  (vram 0x001B7700, 0x138 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// branch-sense / block-layout artifact: outer (a2+8)==2||==0 lowers to beq+bnez vs target's beq+beqz two-branch-to-shared-block, plus epilogue paddub/return-block placement. Body byte-correct. 91.7% on mwcc233; permuter/branch-likely-sense territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS 91.7% (mwcc 2.3.3; 991202 reaches 81.2%). Two-phase state handler keyed
// on the mode word at *(int*)(a2+8) (1, or 2/0). In both phases it sub-dispatches on
// the per-object state byte at *(a1+4) (0/1/2) and always returns 0 unless the state-2
// arm finds the global gate D_00275BD8 clear, in which case it returns 1.
//   mode 1 phase: state 0 -> bump the state byte; state 1 -> set gate D_00275BD8=1,
//     store 1 to D_008106CE, store *(a2+0x14) (no bias) to D_008106CF, bump state;
//     state 2 -> if gate D_00275BD8 == 0 return 1.
//   mode 2/0 phase: state 0 -> bump the state byte; state 1 -> set gate=1, store
//     (*(a2+8)==2 ? 2 : 1) to D_008106CE, store *(a2+0x14)+0x80 to D_008106CF, bump
//     state; state 2 -> if gate == 0 return 1.
// D_00275BD8 is gp-relative (declared scalar under -sdatathreshold 4); D_008106CE/CF
// are over-declared as arrays to force the target's absolute %hi/%lo addressing.
// The inner 0/1/2 dispatch is written as an ascending-case switch so 2.3.3 reverses
// it to the target's 2/1/0 compare order with the case-0 branch-likely fill.
//
// WALL: control-flow lowering artifact -- the outer (a2+8)==2 || ==0 test lowers to
// `beq v1,2 / bnez v1,return` here vs the target's two separate forward branches to
// the shared block (`beq v1,2 / beqz v1`), plus minor epilogue paddub/return-block
// placement. Body and all memory effects are correct; residual is branch-sense /
// block-layout (permuter / branch-likely-sense territory).
extern unsigned char D_00275BD8;
extern unsigned char D_008106CE[];
extern unsigned char D_008106CF[];

int func_001B7700(int a0, char *a1, char *a2) {
    int v1;
    unsigned char s;

    v1 = *(int *)(a2 + 8);
    if (v1 != 1) {
        if (v1 == 2 || v1 == 0) {
            s = *(unsigned char *)(a1 + 4);
            switch (s) {
            case 0:
                *(unsigned char *)(a1 + 4) = s + 1;
                break;
            case 1:
                D_00275BD8 = 1;
                if (*(int *)(a2 + 8) == 2) {
                    D_008106CE[0] = 2;
                } else {
                    D_008106CE[0] = 1;
                }
                D_008106CF[0] = *(unsigned char *)(a2 + 0x14) + 0x80;
                *(unsigned char *)(a1 + 4) = *(unsigned char *)(a1 + 4) + 1;
                break;
            case 2:
                if (D_00275BD8 == 0) {
                    return 1;
                }
                break;
            }
        }
        return 0;
    }
    s = *(unsigned char *)(a1 + 4);
    switch (s) {
    case 0:
        *(unsigned char *)(a1 + 4) = s + 1;
        break;
    case 1:
        D_008106CE[0] = 1;
        D_00275BD8 = 1;
        D_008106CF[0] = *(unsigned char *)(a2 + 0x14);
        *(unsigned char *)(a1 + 4) = *(unsigned char *)(a1 + 4) + 1;
        break;
    case 2:
        if (D_00275BD8 == 0) {
            return 1;
        }
        break;
    }
    return 0;
}
