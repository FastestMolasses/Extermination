// NEARMISS func_00226D50  (vram 0x00226D50, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Cross-branch shared-constant delay-slot fill: target hoists a0=1 into the beqz delay slot (covers both the func_0020CD60(1) call and the later func_001FE9A0(1,...) call) leaving the func_0020CD60 call delay slot as nop; mwcc 2.3.3 re-emits addiu a0,zero,1 in that call slot. Single-instruction sch...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 99.25% on mwcc 2.3.3. Frame-paint dispatch: calls func_001FCBD0 to paint
// a HUD bar, then routes on the input-flag word D_00810E74[0]:
//   bit 0x40 -> func_0020CD40()+func_001FE8D0(), return 1
//   bit 0x20 -> func_0020CD60()+func_001FE8D0(), return 1
//   bit 0x10 -> func_0020CD60(1)+func_001FE8D0(), return 2
// else run the menu-step func_001FE9A0(1, arg0+0x50, arg0+0x54); on 0 return 0,
// otherwise func_001FECB0 over arg0+0x48/0x4C/0x58 and map its result to a code
// (gated by D_00275C58). Logic fully recovered.
// Sole residual: the target hoists the shared constant a0=1 (used by both the
// 0x10-branch call func_0020CD60(1) and the fall-through func_001FE9A0(1,...)) into
// the branch delay slot, leaving the func_0020CD60 call slot a nop; mwcc 2.3.3
// re-materializes a0=1 in the call slot instead. Cross-branch shared-constant
// delay-slot scheduling artifact (permuter-class), NOT the clean-store nop.
extern void func_001FCBD0(int, int, int, int, int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, void *, void *);
extern int func_001FECB0(int, int, void *, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60();
extern int D_00275C58;
extern unsigned short D_00810E74[8];

int func_00226D50(char *arg0) {
    int r;

    func_001FCBD0(0x3C, 0x9B, 7, 0x28, 0x70808080);
    if (D_00810E74[0] & 0x40) {
        func_0020CD40();
        func_001FE8D0();
        return 1;
    }
    if (D_00810E74[0] & 0x20) {
        func_0020CD60();
        func_001FE8D0();
        return 1;
    }
    if (D_00810E74[0] & 0x10) {
        func_0020CD60(1);
        func_001FE8D0();
        return 2;
    }
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    r = func_001FECB0(*(int *)(arg0 + 0x48), *(int *)(arg0 + 0x4C), arg0 + 0x58, 0, 0);
    if (D_00275C58 != 0) {
        return 1;
    }
    if (r == 0) {
        if (*(int *)(arg0 + 0x58) != 2) {
            return 1;
        }
        return 0;
    }
    if (r == -1) {
        return 0;
    }
    return 1;
}
