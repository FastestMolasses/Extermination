// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.73% NEARMISS).
// Fixes read from the original instructions:
//  - The "s7/s8 GPR-coloring wall" on the two loop-hoisted high halves (D_00241B88,
//    D_00241B90) was the spelling of the picture-start exit: the original takes the
//    address of the result word D_0024147C between the two timestamp stores and
//    returns through that pointer. That ordering decides which hoisted base gets
//    which saved register. (Found by the decomp-permuter, confirmed with objdiff.)
//  - Every callee extern was checked against its definition in src/; none was wrong.
//
// Start-code loop (SDK, ee-gcc): each pass calls func_00106AB0, then reads a value with
// func_00106948(0x20). The values are MPEG start-code numbers: 0x1B3 (sequence header)
// calls sub_vertical_size_2800 and 0x1B8 (GOP) calls func_00107178, then the loop goes
// on; 0x1B7 (sequence end) returns 0; 0x100 (picture start) calls func_00106CB0, passes
// a {5, -1, -1} request to func_00109B20(D_002412F4, ...), stores the two returned
// 64-bit fields in D_00241B88 / D_00241B90, and returns D_0024147C. Other values loop.

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
    int *p;

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
            p = &D_0024147C;
            D_00241B88 = st.b;
            return *p;
        }
    }
}
