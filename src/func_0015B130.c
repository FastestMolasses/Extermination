// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: Per-frame tick for the 0x1F0-strided actor/player object `p`.
//   Global byte 0x70003B8D is the "special mode" flag (cutscene/scripted takeover).
//   When it is set:
//     p[5]==0x19          -> latch global byte 0x70003B8F = 1 and fall through to the
//                            normal tick.
//     D_00810700 != 0x15 (stage/level id) && p[0x1F0]==0x2A
//                         -> force p[4]=4, p[5]=0x17, p[6]=0, notify func_00182D70, done.
//     p[0x1F0]==0x17      -> force p[4]=4, p[5]=0xC,  p[6]=0, notify func_00182D70, done.
//     func_00182B30(p)==0 -> force p[4]=4, p[5]=0, p[6]=0, p[0x1F0]=0x41,
//                            func_00174A50(p, 8.0f), notify func_00182D70, done.
//     otherwise           -> fall through to the normal tick.
//   Normal tick: if func_0021C440(p) returns 0, dispatch on the sub-state byte p[5]
//   through jtbl_0026D3B0 (38 entries, 0..37; 37 is an empty case, >=38 skips the
//   dispatch entirely) to one per-state handler each taking (p). State 25 is the only
//   inline case: it calls func_0016DE40(p) when 0x70003B8D is clear, else clears p[1].
//   Afterwards: the 16-bit countdown at p+0x20E, when non-zero, is decremented and
//   p[0]=1 is set on reaching zero; when it is already zero and bit 1 of p[0] is clear,
//   func_0015D100(p) runs. func_0015D000(p) always runs last.

extern unsigned char D_00810700;

extern void func_0015D000(char *);
extern void func_0015D100(char *);
extern void func_00161020(char *);
extern void func_001612D0(char *);
extern void func_00161790(char *);
extern void func_00162190(char *);
extern void func_00162A40(char *);
extern void func_00162DB0(char *);
extern void func_001634A0(char *);
extern void func_001639E0(char *);
extern void func_00163B40(char *);
extern void func_001645D0(char *);
extern void func_001647D0(char *);
extern void func_00165B60(char *);
extern void func_001662D0(char *);
extern void func_00167C80(char *);
extern void func_00168050(char *);
extern void func_00169250(char *);
extern void func_00169730(char *);
extern void func_0016A8B0(char *);
extern void func_0016AC50(char *);
extern void func_0016AE40(char *);
extern void func_0016B790(char *);
extern void func_0016B8A0(char *);
extern void func_0016BC40(char *);
extern void func_0016BF80(char *);
extern void func_0016C6A0(char *);
extern void func_0016D130(char *);
extern void func_0016DE40(char *);
extern void func_0016EBA0(char *);
extern void func_0016EF50(char *);
extern void func_0016FCF0(char *);
extern void func_001703E0(char *);
extern void func_001729A0(char *);
extern void func_00173000(char *);
extern void func_001735C0(char *);
extern void func_00173E60(char *);
extern void func_001741D0(char *);
extern void func_001747F0(char *);
extern void func_00174A50(char *, float);
extern int func_00182B30(char *);
extern void func_00182D70(char *);
extern int func_0021C440(char *);

void func_0015B130(char *p) {
    short t;
    int k;

    if (*(unsigned char *)0x70003B8D != 0) {
        if (*(unsigned char *)(p + 5) == 0x19) {
            *(char *)0x70003B8F = 1;
        } else if (D_00810700 != 0x15 && *(unsigned char *)(p + 0x1F0) == 0x2A) {
            *(char *)(p + 4) = 4;
            *(char *)(p + 5) = 0x17;
            *(char *)(p + 6) = 0;
            func_00182D70(p);
            return;
        } else if (*(unsigned char *)(p + 0x1F0) == 0x17) {
            *(char *)(p + 4) = 4;
            *(char *)(p + 5) = 0xC;
            *(char *)(p + 6) = 0;
            func_00182D70(p);
            return;
        } else if (func_00182B30(p) == 0) {
            *(char *)(p + 4) = 4;
            *(char *)(p + 5) = 0;
            k = 0x41;
            *(char *)(p + 6) = 0;
            *(char *)(p + 0x1F0) = k;
            func_00174A50(p, 8.0f);
            func_00182D70(p);
            return;
        }
    }
    if (func_0021C440(p) == 0) {
        switch (*(unsigned char *)(p + 5)) {
        case 0:
            func_00161020(p);
            break;
        case 1:
            func_001612D0(p);
            break;
        case 2:
            func_00161790(p);
            break;
        case 3:
            func_00162190(p);
            break;
        case 4:
            func_00162A40(p);
            break;
        case 5:
            func_00162DB0(p);
            break;
        case 6:
            func_001634A0(p);
            break;
        case 7:
            func_001639E0(p);
            break;
        case 8:
            func_00163B40(p);
            break;
        case 9:
            func_001647D0(p);
            break;
        case 10:
            func_001645D0(p);
            break;
        case 11:
            func_00165B60(p);
            break;
        case 12:
            func_001662D0(p);
            break;
        case 13:
            func_00167C80(p);
            break;
        case 14:
            func_00168050(p);
            break;
        case 15:
            func_00169250(p);
            break;
        case 16:
            func_00169730(p);
            break;
        case 17:
            func_0016AC50(p);
            break;
        case 18:
            func_0016AE40(p);
            break;
        case 21:
            func_0016A8B0(p);
            break;
        case 19:
            func_0016B790(p);
            break;
        case 20:
            func_0016B8A0(p);
            break;
        case 22:
            func_0016BC40(p);
            break;
        case 23:
            func_0016BF80(p);
            break;
        case 24:
            func_0016D130(p);
            break;
        case 25:
            if (*(unsigned char *)0x70003B8D == 0) {
                func_0016DE40(p);
            } else {
                *(char *)(p + 1) = 0;
            }
            break;
        case 26:
            func_0016EBA0(p);
            break;
        case 27:
            func_0016EF50(p);
            break;
        case 28:
            func_0016C6A0(p);
            break;
        case 29:
            func_0016FCF0(p);
            break;
        case 30:
            func_001703E0(p);
            break;
        case 31:
            func_001729A0(p);
            break;
        case 32:
            func_00173000(p);
            break;
        case 33:
            func_001735C0(p);
            break;
        case 34:
            func_00173E60(p);
            break;
        case 35:
            func_001741D0(p);
            break;
        case 36:
            func_001747F0(p);
            break;
        case 37:
            break;
        }
        t = *(short *)(p + 0x20E);
        if (t != 0) {
            *(short *)(p + 0x20E) = t - 1;
            if (*(short *)(p + 0x20E) == 0) {
                *(char *)(p + 0) = 1;
            }
        } else if (!(*(unsigned char *)(p + 0) & 2)) {
            func_0015D100(p);
        }
        func_0015D000(p);
    }
}
