// NEARMISS func_001FF3F0  (vram 0x001FF3F0, 0x19C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.94% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 87.9% mwcc 2.3.3 (-sdatathreshold 4). Body/4-state control flow byte-correct (gp-rel pointers, lhu half-words, andi counter test all matched). Residual is register allocation of the cached 0x70003B6C slot pointer + address-constant CSE of &D_00289BC0/&D_0028A488 (boot reuses the loaded address re...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// State machine for the active task slot (pointer in scratchpad 0x70003B6C),
// dispatched on the slot's state byte at +0xB (values 0..3). State 0: latches
// the sequence descriptor D_00289BC0 into gp-rel D_00275C70, copies its 0xE
// count into slot+0x14, and if nonzero zeroes the slot+0x16 index and advances
// the state (falling into state 1). State 1: indexes D_0028A488 by slot+0x16<<3,
// calls func_00200780(D_0028A488, D_00275C74, ent[0x20]+desc[4], ent[0x24]) and
// advances the state. State 2: func_00200730() -> 0 stay, 1 advance + bump
// slot+0x16, else step the state back. State 3: func_00200830(D_00275C74),
// decrement slot+0x14; at 0 reset state to 0 and return 1, otherwise reset to 1.
// Returns 1 only on the state-3 completion edge, else 0.
//
// NEARMISS 87.9% with mwcc 2.3.3 (991202 build 81.6%). D_00275C70/74 are gp-rel
// pointer globals (needs -sdatathreshold 4) while D_00289BC0/D_0028A488 are
// arrays (stay absolute); the half-word slot fields are unsigned (lhu) and the
// state-3 counter uses an `n & 0xFFFF` test (matching the boot's andi-reuse).
// Remaining residual is pure compiler artifact: register allocation of the
// cached slot pointer (boot keeps it in $a0/$v1 differently than 2.3.3),
// address-constant CSE of &D_00289BC0/&D_0028A488 (boot reuses the just-loaded
// address register; 2.3.3 rematerializes hi/lo), and branch delay-slot fill
// (boot nop vs 2.3.3 hoisting a lui into the slot). Body/control flow fully
// recovered -> register-allocation / scheduling permuter territory.
extern char *D_00275C70;
extern char *D_00275C74;
extern char D_00289BC0[];
extern char D_0028A488[];

extern void func_00200780(char *a, char *b, int c, int d);
extern int func_00200730(void);
extern void func_00200830(char *a);

int func_001FF3F0(void) {
    char *slot;
    char *desc;
    char *ent;
    int r;
    unsigned int n;

    slot = *(char **)0x70003B6C;
    switch (*(unsigned char *)(slot + 0xB)) {
    case 0:
        desc = D_00289BC0;
        D_00275C70 = desc;
        *(unsigned short *)(slot + 0x14) = *(unsigned short *)(desc + 0xE);
        slot = *(char **)0x70003B6C;
        if (*(unsigned short *)(slot + 0x14) == 0) {
            return 0;
        }
        *(short *)(slot + 0x16) = 0;
        slot = *(char **)0x70003B6C;
        *(unsigned char *)(slot + 0xB) = *(unsigned char *)(slot + 0xB) + 1;
        /* fall through */
    case 1:
        slot = *(char **)0x70003B6C;
        desc = D_00275C70;
        ent = (char *)((*(unsigned short *)(slot + 0x16) << 3) + (int)D_0028A488);
        func_00200780(D_0028A488,
                      D_00275C74,
                      *(int *)(ent + 0x20) + *(int *)(desc + 4),
                      *(int *)(ent + 0x24));
        slot = *(char **)0x70003B6C;
        *(unsigned char *)(slot + 0xB) = *(unsigned char *)(slot + 0xB) + 1;
        break;
    case 2:
        r = func_00200730();
        if (r == 0) break;
        if (r == 1) {
            slot = *(char **)0x70003B6C;
            *(unsigned char *)(slot + 0xB) = *(unsigned char *)(slot + 0xB) + 1;
            slot = *(char **)0x70003B6C;
            *(unsigned short *)(slot + 0x16) = *(unsigned short *)(slot + 0x16) + 1;
            break;
        }
        slot = *(char **)0x70003B6C;
        *(unsigned char *)(slot + 0xB) = *(unsigned char *)(slot + 0xB) - 1;
        break;
    case 3:
        func_00200830(D_00275C74);
        slot = *(char **)0x70003B6C;
        n = *(unsigned short *)(slot + 0x14) - 1;
        *(short *)(slot + 0x14) = n;
        if ((n & 0xFFFF) == 0) {
            slot = *(char **)0x70003B6C;
            *(unsigned char *)(slot + 0xB) = 0;
            return 1;
        }
        slot = *(char **)0x70003B6C;
        *(unsigned char *)(slot + 0xB) = 1;
        break;
    }
    return 0;
}
