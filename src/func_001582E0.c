// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.43% NEARMISS).
// Fixes read from the original instructions:
//  - func_001B0FD0 and func_001AFC10 take only the self pointer (their src/
//    definitions); the old 3-argument / (self, state) externs were wrong.
//  - The case-1 table bit is (1 << byte at +0x2E) (a byte load), while the
//    case-0 test uses the halfword at +0x2E masked to 8 bits.
//  - Spelled like the byte-matched twin func_00158430: if/else arms with
//    break, and stores through unsigned char.
//
// Per-state dispatch on the state byte at arg0+4. State 0: if the global
// bitmask D_00810841[D_00810700] already has bit (1<<*(u16*)(arg0+0x2E)) set,
// go to state 3; else call func_001B0FD0(self) (result ignored),
// func_001C6380(self), set *arg0=1 and start func_001F1110(arg0,2). State 1:
// when *(s16*)(arg0+0x36) is set, mark +0/+4 = 2, set the bitmask bit, OR 2
// into D_00810842 and call func_001FB9F0(0x3F1,0x1000,0x1000,0x1000); then
// always run func_001F1180, func_001B17A0 and the virtual at arg0+0x4C.
// States 2/3: func_001AFC10(self).
extern void func_001AFC10(char *p);
extern int func_001B0FD0(char *p);
extern void func_001C6380(char *p);
extern void func_001F1110(char *p, int a);
extern void func_001F1180(char *p);
extern void func_001B17A0(char *p);
extern int func_001FB9F0(int a, int b, int c, int d);
extern unsigned char D_00810700;
extern unsigned char D_00810841;
extern unsigned char D_00810842;

void func_001582E0(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        if ((&D_00810841)[D_00810700] & ((1 << *(unsigned short *)(arg0 + 0x2E)) & 0xFF)) {
            *(unsigned char *)(arg0 + 4) = 3;
        } else {
            func_001B0FD0(arg0);
            func_001C6380(arg0);
            *(unsigned char *)(arg0 + 0) = 1;
            func_001F1110(arg0, 2);
        }
        break;
    case 1:
        if (*(short *)(arg0 + 0x36) != 0) {
            *(unsigned char *)(arg0 + 0) = 2;
            *(unsigned char *)(arg0 + 4) = 2;
            (&D_00810841)[D_00810700] |= 1 << *(unsigned char *)(arg0 + 0x2E);
            D_00810842 |= 2;
            func_001FB9F0(0x3F1, 0x1000, 0x1000, 0x1000);
        }
        func_001F1180(arg0);
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
