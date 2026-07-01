// NEARMISS func_001607D0  (vram 0x001607D0, 0x850 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body byte-identical; SOLE residual class (15 uniform sites) = mwcc 2.3.3 speculates the literal `lui at,0x7000` (address half of the NEXT absolute pad-register lhu, first instr of the branch-taken successor) into beqz/bnez delay slots and retargets the branch +4, where the pinned 2.3.1.01 leaves ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Player/actor input state machine on the state byte at +0x1F0.
// Reads pad bits at absolute IOP addresses 0x70003B74..0x70003B7E masked by
// the global event flags D_00810E70 (held) / D_00810E74 (pressed) and either
// latches a new anim state (+5), clears the sub-state (+6/+0x1F1), or
// forwards to the direction/step handlers func_0017A8B0/func_0017A970/
// func_0017AAD0. +0x1FC is reset to 1.0f every tick.
extern unsigned char D_00810C61;
extern unsigned short D_00810E70;
extern unsigned short D_00810E74;
extern void func_0016F5D0(char *obj);
extern int func_0017A8B0(char *arg0, int arg1);
extern int func_0017A970(char *arg0, int arg1);
extern int func_0017AAD0(char *arg0);
extern void func_0017C370(char *arg0);

int func_001607D0(char *p) {
    unsigned char st;

    *(float *)(p + 0x1FC) = 1.0f;
    st = *(unsigned char *)(p + 0x1F0);
    switch (st) {
    case 0x00:
        if (D_00810E70 & *(unsigned short *)0x70003B7E) {
            if (*(unsigned char *)(p + 0x236) != 0) {
                goto ret0;
            }
            *(char *)(p + 5) = 0x1E;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x32;
            *(char *)(p + 0x1F1) = 0;
            return 1;
        }
        if (D_00810E70 & *(unsigned short *)0x70003B7C) {
            if (*(unsigned char *)(p + 0x236) != 0) {
                goto ret0;
            }
            *(char *)(p + 5) = 0x1D;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x31;
            *(char *)(p + 0x1F1) = 0;
            return 1;
        }
        if (D_00810E74 & *(unsigned short *)0x70003B78) {
            *(char *)(p + 5) = 0x21;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x36;
            return 1;
        }
        if (D_00810E74 & *(unsigned short *)0x70003B74) {
            *(char *)(p + 5) = 0x22;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x37;
            return 1;
        }
        goto ret0;
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x07:
    case 0x06:
        if (D_00810E70 & *(unsigned short *)0x70003B7E) {
            if (*(unsigned char *)(p + 0x236) != 0) {
                goto ret0;
            }
            func_0017C370(p);
            *(char *)(p + 5) = 0x1E;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x32;
            *(char *)(p + 0x1F1) = 0;
            return 1;
        }
        if (D_00810E70 & *(unsigned short *)0x70003B7C) {
            if (*(unsigned char *)(p + 0x236) != 0) {
                goto ret0;
            }
            func_0017C370(p);
            *(char *)(p + 5) = 0x1D;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x31;
            *(char *)(p + 0x1F1) = 0;
            return 1;
        }
        if (D_00810E74 & *(unsigned short *)0x70003B78) {
            func_0017C370(p);
            *(char *)(p + 5) = 0x21;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x36;
            return 1;
        }
        if (D_00810E74 & *(unsigned short *)0x70003B74) {
            func_0017C370(p);
            *(char *)(p + 5) = 0x22;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x37;
            return 1;
        }
        goto ret0;
    case 0x31:
        if (D_00810E70 & *(unsigned short *)0x70003B7E) {
            *(char *)(p + 5) = 0x1E;
            *(unsigned char *)(p + 0x1F0) = 0x32;
            *(char *)(p + 0x318) = 1;
        } else if (!(D_00810E70 & *(unsigned short *)0x70003B7C)) {
            *(char *)(p + 6) = 0x63;
            func_0016F5D0(p);
            return 1;
        }
        if (D_00810E74 & *(unsigned short *)0x70003B78) {
            if (D_00810C61 != 0) {
                goto ret0;
            }
            return func_0017A8B0(p, 0);
        }
        if (D_00810E70 & *(unsigned short *)0x70003B78) {
            if (D_00810C61 == 0) {
                goto ret0;
            }
            return func_0017A8B0(p, 0);
        }
        if (D_00810E74 & *(unsigned short *)0x70003B74) {
            return func_0017A970(p, 0);
        }
        if (D_00810E70 & *(unsigned short *)0x70003B74) {
            return func_0017A970(p, 1);
        }
        if (D_00810E74 & *(unsigned short *)0x70003B76) {
            if (func_0017AAD0(p) == 0) {
                goto ret0;
            }
            return 1;
        }
        goto ret0;
    case 0x32:
        if (!(D_00810E70 & *(unsigned short *)0x70003B7E)) {
            if (D_00810E70 & *(unsigned short *)0x70003B7C) {
                *(char *)(p + 5) = 0x1D;
                *(unsigned char *)(p + 0x1F0) = 0x31;
                *(char *)(p + 0x318) = 1;
                goto ret0;
            }
            *(char *)(p + 6) = 0x63;
            func_0016F5D0(p);
            return 1;
        }
        if (D_00810E74 & *(unsigned short *)0x70003B78) {
            if (D_00810C61 != 0) {
                goto ret0;
            }
            return func_0017A8B0(p, 0);
        }
        if (D_00810E70 & *(unsigned short *)0x70003B78) {
            if (D_00810C61 == 0) {
                goto ret0;
            }
            return func_0017A8B0(p, 0);
        }
        if (D_00810E74 & *(unsigned short *)0x70003B74) {
            return func_0017A970(p, 0);
        }
        if (D_00810E70 & *(unsigned short *)0x70003B74) {
            return func_0017A970(p, 1);
        }
        if (D_00810E74 & *(unsigned short *)0x70003B76) {
            if (func_0017AAD0(p) == 0) {
                goto ret0;
            }
            return 1;
        }
        goto ret0;
    case 0x33:
        if (D_00810E70 & *(unsigned short *)0x70003B7C) {
            goto ret0;
        }
        if (D_00810E70 & *(unsigned short *)0x70003B7E) {
            goto ret0;
        }
        return 1;
    case 0x27:
        if (D_00810E70 & *(unsigned short *)0x70003B7E) {
            *(char *)(p + 5) = 0x20;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x35;
            *(char *)(p + 0x1F1) = 0;
            return 1;
        }
        if (D_00810E70 & *(unsigned short *)0x70003B7C) {
            *(char *)(p + 5) = 0x1F;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x34;
            *(char *)(p + 0x1F1) = 0;
            return 1;
        }
        goto ret0;
    case 0x34:
        if (D_00810E70 & *(unsigned short *)0x70003B7E) {
            *(char *)(p + 5) = 0x20;
            *(unsigned char *)(p + 0x1F0) = 0x35;
            *(char *)(p + 0x318) = 1;
            goto ret0;
        }
        if (!(D_00810E70 & *(unsigned short *)0x70003B7C)) {
            *(char *)(p + 6) = 0x63;
            func_0016F5D0(p);
            return 1;
        }
        if (D_00810E74 & *(unsigned short *)0x70003B78) {
            if (D_00810C61 != 0) {
                goto ret0;
            }
            return func_0017A8B0(p, 0);
        }
        if (D_00810E70 & *(unsigned short *)0x70003B78) {
            if (D_00810C61 == 0) {
                goto ret0;
            }
            return func_0017A8B0(p, 0);
        }
        if (D_00810E74 & *(unsigned short *)0x70003B74) {
            return func_0017A970(p, 0);
        }
        if (D_00810E70 & *(unsigned short *)0x70003B74) {
            return func_0017A970(p, 1);
        }
        if (D_00810E74 & *(unsigned short *)0x70003B76) {
            if (func_0017AAD0(p) == 0) {
                goto ret0;
            }
            return 1;
        }
        goto ret0;
    case 0x35:
        if (!(D_00810E70 & *(unsigned short *)0x70003B7E)) {
            if (D_00810E70 & *(unsigned short *)0x70003B7C) {
                *(char *)(p + 5) = 0x1F;
                *(unsigned char *)(p + 0x1F0) = 0x34;
                *(char *)(p + 0x318) = 1;
                goto ret0;
            }
            *(char *)(p + 6) = 0x63;
            func_0016F5D0(p);
            return 1;
        }
        if (D_00810E74 & *(unsigned short *)0x70003B78) {
            if (D_00810C61 != 0) {
                goto ret0;
            }
            return func_0017A8B0(p, 0);
        }
        if (D_00810E70 & *(unsigned short *)0x70003B78) {
            if (D_00810C61 == 0) {
                goto ret0;
            }
            return func_0017A8B0(p, 0);
        }
        if (D_00810E74 & *(unsigned short *)0x70003B74) {
            return func_0017A970(p, 0);
        }
        if (D_00810E70 & *(unsigned short *)0x70003B74) {
            return func_0017A970(p, 1);
        }
        if (D_00810E74 & *(unsigned short *)0x70003B76) {
            if (func_0017AAD0(p) == 0) {
                goto ret0;
            }
            return 1;
        }
        goto ret0;
    case 0x36:
    case 0x37:
        goto ret0;
    }
ret0:
    return 0;
}
