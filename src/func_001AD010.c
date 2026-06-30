// NEARMISS func_001AD010  (vram 0x001AD010, 0x12C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.45% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW instruction-scheduling + CW-specific redundant mask (same family as sibling func_001ACEC0 parked at 88.8%): CW hoists the D_00810702 store across the slot-ptr load and fills call/branch delay slots with the slot+0xB store interleaved with arg loads; CW also emits a redundant andi v1,v1,0xff af...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS func_001AD010 (vram 0x001AD010, 0x12C bytes) — readable decompilation,
// NOT byte-identical (best 83.45% via mwcc 2.3.3; 991202 77.27%).
//
// SEMANTICS: input/pad-state transition handler (sibling of func_001ACEC0).
// The active frame-task slot ptr lives in scratchpad word 0x70003B6C; bytes
// +9/+0xA/+0xB are sub-state fields. Reads gp/abs flag bytes D_008106B5..B8
// (pad/config snapshot) and writes D_00810700..702 (committed pad state).
//   if (D_008106B8 == 2): D_00810702 = D_008106B7; slot[+0xB] = 4; return.
//   st = *(u8*)0x70003B93;  (a pending-transition request byte)
//   if (st != 0): if (st==2) func_001FABB0();  slot[+9]=3; slot[+0xA]=0;
//                 slot[+0xB]=0; return.
//   else (st==0): D_00810700 = D_008106B5; D_00810702 = D_008106B7;
//                 D_00810701 = (D_008106B6==0xFF) ? (D_00810730[D_008106B5]&0x7F)
//                                                 : D_008106B6;
//                 slot[+9]=5; slot[+0xA]=0; slot[+0xB]=0;
//                 func_001FBC50(D_008106B7, D_008106B5); func_001FABB0();
//
// WALL (CW instruction-scheduling + redundant mask — same family as the
// func_001ACEC0 88.8% wall): body/logic fully recovered. Residuals:
//  (1) In the D_008106B8==2 arm CW hoists the D_00810702 store across the
//      0x70003B6C slot-ptr load (lui/lbu/lui/sb interleaved); mwcc keeps the
//      store adjacent to its load — statement reorder does not move it.
//  (2) CW fills the func_00206B00/func_001FBC50 call delay slots and the b's
//      delay slots with the slot[+0xB]=0 store and interleaves the B7/B5 arg
//      loads; mwcc schedules them as separate ordered statements.
//  (3) CW emits a redundant `andi v1,v1,0xff` after the unsigned `lbu`
//      0x70003B93 before the ==2 compare; mwcc proves it unnecessary and drops
//      it. CW-specific codegen; not reachable from C.
extern void func_001FABB0(void);
extern void func_001FBC50(int, int);
extern unsigned char D_008106B5;
extern unsigned char D_008106B6;
extern unsigned char D_008106B7;
extern unsigned char D_008106B8;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810730[];

void func_001AD010(void) {
    unsigned char st;

    if (D_008106B8 == 2) {
        D_00810702 = D_008106B7;
        *(unsigned char *)(*(int *)0x70003B6C + 0xB) = 4;
        return;
    }
    st = *(unsigned char *)0x70003B93;
    if (st != 0) {
        if (st == 2) {
            func_001FABB0();
        }
        *(unsigned char *)(*(int *)0x70003B6C + 9) = 3;
        *(unsigned char *)(*(int *)0x70003B6C + 0xA) = 0;
        *(unsigned char *)(*(int *)0x70003B6C + 0xB) = 0;
        return;
    }
    D_00810700 = D_008106B5;
    D_00810702 = D_008106B7;
    if (D_008106B6 == 0xFF) {
        D_00810701 = D_00810730[D_008106B5] & 0x7F;
    } else {
        D_00810701 = D_008106B6;
    }
    *(unsigned char *)(*(int *)0x70003B6C + 9) = 5;
    *(unsigned char *)(*(int *)0x70003B6C + 0xA) = 0;
    *(unsigned char *)(*(int *)0x70003B6C + 0xB) = 0;
    func_001FBC50(D_008106B7, D_008106B5);
    func_001FABB0();
}
