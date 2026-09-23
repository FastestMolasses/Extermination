// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// MATCH NOTE (m1-firstlevel-matching lane): the former 95.5% "scheduling"
// residual came from wrong callee prototypes. func_001FBC50 takes no arguments,
// func_001FF080 takes (int, int) and func_001AEE10 takes (short, unsigned char),
// as their byte-matched definitions in src/ show. The spurious extra constant
// arguments moved the argument setup and the jal delay-slot store.
// D_00275BD8 is now declared extern. The tentative definition emitted a local
// .sbss object.

// State machine on global D_00810040 (byte[0]=main state, byte[1]=sub-state).
// The [64] array types on the externs are a matching device: they keep the large
// globals out of .sdata (forcing %hi/%lo) at sdatathreshold 8 while the 1-byte
// D_00275BD8 still resolves gp-relative, matching the original codegen.
extern void func_001FBC50(void);
extern int func_001FABB0(void);
extern void func_001FF080(int, int);
extern int func_001AEDE0(int, int);
extern void func_001AEE10(short, unsigned char);
extern int func_0020A7A0(long long);
extern int func_00225720(unsigned char *);
extern unsigned char func_00225D20(unsigned char *);
extern int func_00200970(int);
extern unsigned char D_00810040[64];
extern unsigned char D_00275BD8;
extern signed char D_00282157[64];
extern short D_0028A9A0[64];

unsigned char func_00225AC0(int arg0) {
    unsigned char *p = D_00810040;

    switch (p[0]) {
    case 0:
        switch (p[1]) {
        case 0:
            switch (arg0) {
            case 0:
                p[0x14] = 2;
                func_001FBC50();
                func_001FABB0();
                p[1] = p[1] + 1;
                break;
            case 1:
                p[0x14] = 1;
                p[1] = 2;
                break;
            }
            break;
        case 1:
            if (D_00282157[0] != 0) {
                break;
            }
            p[1] = p[1] + 1;
        case 2:
            p[1] = p[1] + 1;
            D_00275BD8 = 1;
            func_001FF080(0, 0x2A);
            break;
        case 3:
            if (D_00275BD8 == 0) {
                p[0] = p[0] + 1;
                p[1] = 0;
                func_001AEE10(8, 0);
            }
            break;
        }
        goto d0;
    case 1:
        func_0020A7A0(((long long)0x20044C05 << 0x20) | ((long long)0x9D42 << 0x10) | 0x21E0);
        func_00225720(p);
        p[0x16] = func_00225D20(p);
        if (p[0x16] != 0) {
            func_001AEDE0(8, 0);
            p[0] = 2;
        }
        goto d0;
    case 2:
        func_0020A7A0(((long long)0x20044C05 << 0x20) | ((long long)0x9D42 << 0x10) | 0x21E0);
        func_00225720(p);
        if (D_0028A9A0[0] == 2) {
            if (*(unsigned char *)0x70003B90 == 2) {
                func_00200970(1);
            }
            p[0] = 3;
        }
        goto d0;
    case 3:
        return p[0x16];
    }
d0:
    return 0;
}
