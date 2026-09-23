// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003B93

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
//                 func_001FBC50(b7, b5); func_001FABB0();
//
// MATCH NOTE (m2-matching lane, 83.45% -> 100.0%). The recorded "CW-only"
// residuals were two source facts:
//  (1) The request byte 0x70003B93 is a relocated scratchpad extern
//      (idiom-32), opted in per file by `// SPAD: 0x70003B93`. As a literal,
//      mwcc speculated its `lui at,0x7000` into the D_008106B8 branch slot, and
//      the redundant-looking `andi v1,v1,0xff` did not appear.
//  (2) B5/B7 are read once into locals, and func_001FBC50 is called with them
//      as `unsigned char` arguments. The target keeps both bytes in $a1/$a0
//      from their first load to the call. The callee's byte-matched definition
//      reads no arguments, but this caller passes them (see MATCHING_GUIDE:
//      an ignoring callee does not prove the caller passed none).
extern int D_70003B6C;                              /* PS2 scratchpad @ 0x70003B6C */

extern void func_001FABB0(void);
extern void func_001FBC50(unsigned char, unsigned char);
extern unsigned char D_008106B5;
extern unsigned char D_008106B6;
extern unsigned char D_008106B7;
extern unsigned char D_008106B8;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810730[];
extern unsigned char D_70003B93;

void func_001AD010(void) {
    unsigned char st;
    unsigned char b5;
    unsigned char b7;

    if (D_008106B8 == 2) {
        D_00810702 = D_008106B7;
        *(unsigned char *)(D_70003B6C + 0xB) = 4;
        return;
    }
    st = D_70003B93;
    if (st != 0) {
        if (st == 2) {
            func_001FABB0();
        }
        *(unsigned char *)(D_70003B6C + 9) = 3;
        *(unsigned char *)(D_70003B6C + 0xA) = 0;
        *(unsigned char *)(D_70003B6C + 0xB) = 0;
        return;
    }
    b5 = D_008106B5;
    b7 = D_008106B7;
    D_00810700 = b5;
    D_00810702 = b7;
    if (D_008106B6 == 0xFF) {
        D_00810701 = D_00810730[b5] & 0x7F;
    } else {
        D_00810701 = D_008106B6;
    }
    *(unsigned char *)(D_70003B6C + 9) = 5;
    *(unsigned char *)(D_70003B6C + 0xA) = 0;
    *(unsigned char *)(D_70003B6C + 0xB) = 0;
    func_001FBC50(b7, b5);
    func_001FABB0();
}
