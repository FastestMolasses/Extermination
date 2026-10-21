// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Flag-dispatch handler. Copies a 0x20-byte template (eight words) from
// D_00248710 into a stack buffer, then branches on arg1:
//  bit0 set: when the global entity at *(0x700031D4) is non-null and its flag
//    byte (e+2, masked ~0xE0) is 2, run func_00102948(D_00281B50, D_700031C0)
//    and r = func_0019AD00(arg0, arg2, 6); if (r & 6) tail to
//    func_00176BE0(arg0), else when neither D_00275B00[4] nor arg0[0x314]
//    intersects the buffer word buf[arg3], call func_00176180(arg0, r, arg2).
//    A null entity or another flag value runs func_00176BE0(arg0).
//  bit1 set: if func_001762E0(arg0) == 0, call func_00176BE0(arg0).
//  otherwise: func_00176BE0(arg0).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 5, 2026-10-20). The
// 91.67 NEARMISS "backend artifacts" were source errors: func_001762E0 and the
// last func_00176BE0 take arg0 (the original calls them with its a0 untouched;
// the old C passed nothing), the two fallback calls are one else branch
// (`e != 0 && ... == 2`), and the template is a word array indexed directly
// (buf.w[arg3]), which gives the target's sp-relative load.
typedef int s128 __attribute__((mode(TI)));
struct q2 { int w[8]; };
extern void func_00102948(void *dst, void *src);
extern void func_00176180(char *p, s128 v, int n);
extern int func_001762E0(char *p);
extern void func_00176BE0(char *actor);
extern s128 func_0019AD00(char *p, int a, int b);
extern struct q2 D_00248710;
extern unsigned char D_00275B00;
extern char D_00281B50[];
extern char D_700031C0[];

void func_00176390(char *arg0, int arg1, int arg2, int arg3) {
    struct q2 buf;

    buf = D_00248710;

    if (arg1 & 1) {
        char *e = *(char **)0x700031D4;
        if (e != 0 && (*(unsigned char *)(e + 2) & ~0xE0) == 2) {
            {
                s128 r;
                func_00102948(D_00281B50, D_700031C0);
                r = func_0019AD00(arg0, arg2, 6);
                if (r & 6) {
                    func_00176BE0(arg0);
                    return;
                }
                {
                    int m = buf.w[arg3];
                    if (!((&D_00275B00)[4] & m) && !(*(unsigned char *)(arg0 + 0x314) & m)) {
                        func_00176180(arg0, r, arg2);
                    }
                }
            }
        } else {
            func_00176BE0(arg0);
        }
    } else if (arg1 & 2) {
        if (func_001762E0(arg0) == 0) {
            func_00176BE0(arg0);
        }
    } else {
        func_00176BE0(arg0);
    }
}
