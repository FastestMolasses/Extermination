// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.03% NEARMISS).
// Fixes read from the original instructions:
//  - States 2 and 3 share one case body (`case 2: case 3:`); two separate
//    identical cases emitted two calls instead of the original's shared
//    func_001AFC10 tail. The old "register-coloring" verdict was wrong: the
//    body and registers already matched.
//
// Per-actor state step dispatched on the state byte at *(a0+4) (states 0/1/2/3):
//   state 0: test the per-actor bit in the shared bitmap D_00810841[D_00810700],
//            indexed by *(u16*)(a0+0x2e). If set, advance state to 3. Else run the
//            spawn path: func_001B0FD0(self); func_001C6380(self); state byte at a0+0 = 1;
//            func_001F1110(self, 2).
//   state 1: if the short at a0+0x36 is nonzero, set a0+0 = 2 and a0+4 = 2, OR the
//            actor bit (1 << *(u8*)(a0+0x2e)) into D_00810841[D_00810700], and fire
//            func_001FB9F0(0x3F1, 0x1000, 0x1000, 0x1000). Either way then run
//            func_001F1180(self); func_001B17A0(self); and the virtual at *(a0+0x4C)(self).
//   states 2 and 3: call func_001AFC10(self).
// (Checked 2026-09-27 against the original-instruction harness: state 0 passes
// the record to func_001B0FD0.)
extern unsigned char D_00810700;
extern unsigned char D_00810841[];

extern void func_001AFC10(unsigned char *self);
extern int func_001B0FD0(int self);
extern void func_001C6380(int self);
extern void func_001F1110(int self, int a);
extern void func_001FB9F0(int a, int b, int c, int d);
extern void func_001F1180(int self);
extern void func_001B17A0(int self);

void func_001581A0(int a0) {
    int s0;
    unsigned char st;
    short h;

    s0 = a0;
    st = *(unsigned char *)(a0 + 4);
    switch (st) {
    case 0:
        if (D_00810841[D_00810700] & (unsigned char)(1 << *(unsigned short *)(s0 + 0x2e))) {
            *(char *)(s0 + 4) = 3;
        } else {
            func_001B0FD0(s0);
            func_001C6380(s0);
            *(char *)(s0 + 0) = 1;
            func_001F1110(s0, 2);
        }
        break;
    case 1:
        h = *(short *)(s0 + 0x36);
        if (h != 0) {
            *(char *)(s0 + 0) = 2;
            *(char *)(s0 + 4) = 2;
            D_00810841[D_00810700] |= (unsigned char)(1 << *(unsigned char *)(s0 + 0x2e));
            func_001FB9F0(0x3F1, 0x1000, 0x1000, 0x1000);
        }
        func_001F1180(s0);
        func_001B17A0(s0);
        (*(void (**)(int))(s0 + 0x4C))(s0);
        break;
    case 2:
    case 3:
        func_001AFC10((unsigned char *)s0);
        break;
    }
}
