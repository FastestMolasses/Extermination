// NEARMISS func_00106B88  (vram 0x00106B88, 0x124 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.73% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall (confirmed s84): body byte-identical and .text size exact (0x124 == expected 0x124); residual is 4 instructions where s7 and s8 are swapped on the two loop-hoisted %hi bases (lui s8,%hi(D_00241B90)/lui s7,%hi(D_00241B88) vs ours reversed, and the two matching sd). Register...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void func_00106AB0(void);
extern unsigned int func_00106948(int mask);
extern void sub_vertical_size_2800(void);
extern void func_00107178(void);
extern void func_00106CB0(void);
extern void func_00109B20(int arg, void *p);

extern int D_002412F4;
extern long D_00241B88;
extern long D_00241B90;
extern int D_0024147C;

int func_00106B88(void) {
    struct {
        int a;        /* 0x00 */
        long b;       /* 0x08 */
        long c;       /* 0x10 */
    } st;
    unsigned int ev;

    for (;;) {
        func_00106AB0();
        ev = func_00106948(0x20);
        switch (ev) {
        case 0x1B3:
            sub_vertical_size_2800();
            break;
        case 0x1B8:
            func_00107178();
            break;
        case 0x1B7:
            return 0;
        case 0x100:
            func_00106CB0();
            st.a = 5;
            st.b = -1;
            st.c = -1;
            func_00109B20(D_002412F4, &st);
            D_00241B90 = st.c;
            D_00241B88 = st.b;
            return D_0024147C;
        }
    }
}
