// NEARMISS func_00176390  (vram 0x00176390, 0x14C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// backend artifacts: (1) target speculates paddub a0,s2 into the 'if(e==NULL)' beqz delay slot (idiom-13/14 fill) where mwcc leaves a nop; (2) e+2 byte read colored to a0 in target, a1 in mwcc; (3) sp+offset address folding on the buf[arg3] load (addu a0,v1,sp + lw 0x40(a0) vs addiu a0,sp,0x40 + ad...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 91.7% (mwcc233). Flag-dispatch handler. Copies a 0x20-byte template
// (two quadwords) from D_00248710 into a stack buffer, then branches on arg1:
//  bit0 set: look up the global entity at *(0x700031D4); if non-null and its
//    flag byte (e+2, masked ~0xE0) == 2, run func_00102948(D_00281B50,D_700031C0)
//    and func_0019AD00(arg0,arg2,6) -> r; if (r & 6) tail to func_00176BE0(arg0),
//    else if neither D_00275B00[4] nor arg0[0x314] intersects the buffer slot
//    buf[arg3], call func_00176180(arg0,r,arg2). Non-matching flag / null entity
//    fall to func_00176BE0(arg0).
//  bit1 set: if func_001762E0() == 0, call func_00176BE0(arg0).
//  otherwise: func_00176BE0().
// Quadword copy + full branch structure byte-identical to target; residuals are a
// beqz delay-slot speculation, an a0/a1 byte-load coloring, and sp-offset address
// folding on the buf[arg3] load — all backend artifacts.
typedef int s128 __attribute__((mode(TI)));
struct q2 { s128 a; s128 b; };
extern void func_00102948(void *dst, void *src);
extern void func_00176180(char *p, s128 v, int n);
extern int func_001762E0();
extern void func_00176BE0();
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
        if (e != 0) {
            if ((*(unsigned char *)(e + 2) & ~0xE0) == 2) {
                s128 r;
                func_00102948(D_00281B50, D_700031C0);
                r = func_0019AD00(arg0, arg2, 6);
                if (r & 6) {
                    func_00176BE0(arg0);
                    return;
                }
                {
                    int m = *(int *)((char *)&buf + arg3 * 4);
                    if (!((&D_00275B00)[4] & m) && !(*(unsigned char *)(arg0 + 0x314) & m)) {
                        func_00176180(arg0, r, arg2);
                    }
                }
            } else {
                func_00176BE0(arg0);
            }
        } else {
            func_00176BE0(arg0);
        }
    } else if (arg1 & 2) {
        if (func_001762E0() == 0) {
            func_00176BE0(arg0);
        }
    } else {
        func_00176BE0();
    }
}
