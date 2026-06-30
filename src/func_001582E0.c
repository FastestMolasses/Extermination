// NEARMISS func_001582E0  (vram 0x001582E0, 0x150 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation + scheduling near-miss (best on mwcc 2.3.3, 96.4%; 991202 only 83.7%). Body/structure fully correct. Residuals: (1) GPR coloring of the (1<<x)&0xFF bit chain — target threads it through t0/a3/a2 and reuses the case-3 compare constant 3, mine is shifted one register; (2) two de...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-state dispatch on the state byte at arg0+4. State 0: if the global
// bitmask D_00810841[D_00810700] already has bit (1<<*(u16*)(arg0+0x2E)) set,
// jump straight to state 3; else kick func_001B0FD0 / func_001C6380, set
// *arg0=1, and start func_001F1110(arg0,2). State 1: when *(s16*)(arg0+0x36)
// is set, advance to state 2, set the bitmask bit, OR 2 into D_00810842 and
// call func_001FB9F0(0x3F1,0x1000,0x1000,0x1000); then always run
// func_001F1180, func_001B17A0 and the virtual at arg0+0x4C. States 2/3:
// func_001AFC10(arg0, state).
//
// NEARMISS 96.4% (mwcc 2.3.3 mwcps2-2.3.3-000906; 991202 reaches 83.7%). Logic
// fully recovered; sole residual is GPR coloring of the (1<<x)&0xFF bit chain
// (target threads it t0/a3/a2; mine shifted one reg) plus two delay-slot fills
// the target schedules: the case-2 dispatch as a branch-likely (beql) and the
// case-0 beqz slot filled with the speculatable %hi(D_00810700) load. Permuter
// class (register-allocation / scheduling).
extern void func_001AFC10(char *p, unsigned char st);
extern void func_001B0FD0(int a, int b, int c);
extern void func_001B17A0(char *p);
extern void func_001C6380(char *p);
extern void func_001F1110(char *p, int a);
extern void func_001F1180(char *p);
extern void func_001FB9F0(int a, int b, int c, int d);
extern unsigned char D_00810700;
extern unsigned char D_00810841;
extern unsigned char D_00810842;

void func_001582E0(char *arg0) {
    unsigned char st;
    int sh;
    int bit;

    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        sh = 1 << *(unsigned short *)(arg0 + 0x2E);
        bit = sh & 0xFF;
        if ((&D_00810841)[D_00810700] & bit) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        func_001B0FD0(bit, sh, 1);
        func_001C6380(arg0);
        *(char *)(arg0 + 0) = 1;
        func_001F1110(arg0, 2);
        return;
    case 1:
        if (*(short *)(arg0 + 0x36) != 0) {
            *(char *)(arg0 + 0) = 2;
            *(unsigned char *)(arg0 + 4) = 2;
            (&D_00810841)[D_00810700] |= 1 << *(unsigned short *)(arg0 + 0x2E);
            D_00810842 |= 2;
            func_001FB9F0(0x3F1, 0x1000, 0x1000, 0x1000);
        }
        func_001F1180(arg0);
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    case 2:
    case 3:
        func_001AFC10(arg0, st);
        return;
    }
}
