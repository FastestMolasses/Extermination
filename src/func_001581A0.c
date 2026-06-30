// NEARMISS func_001581A0  (vram 0x001581A0, 0x13C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-coloring permutation: body byte-correct, residual is the (1<<x) shift-base constant in $a3 (target) vs $a2 (here) threaded through the bit-table indexing, plus the state-2 beql branch-likely / func_001AFC10 tail placement. 91.1% on mwcc233; permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 91.1% (mwcc 2.3.3; 991202 reaches 80.2%). Per-actor state step dispatched
// on the state byte at *(a0+4) (states 0/1/2/3):
//   state 0: test the per-actor bit in the shared bitmap D_00810841[D_00810700],
//            indexed by *(u16*)(a0+0x2e). If set, advance state to 3. Else run the
//            spawn path: func_001B0FD0(); func_001C6380(self); state byte at a0+0 = 1;
//            func_001F1110(self, 2).
//   state 1: if the short at a0+0x36 is nonzero, set a0+0 = 2 and a0+4 = 2, OR the
//            actor bit (1 << *(u8*)(a0+0x2e)) into D_00810841[D_00810700], and fire
//            func_001FB9F0(0x3F1, 0x1000, 0x1000, 0x1000). Either way then run
//            func_001F1180(self); func_001B17A0(self); and the virtual at *(a0+0x4C)(self).
//   states 2 and 3: tail-call func_001AFC10(self).
// The 0/1/2/3 dispatch is a switch so 2.3.3 reproduces the target's beq/beql descending
// compare chain (states 3 and 2 reached via the beql branch-likely sharing the
// func_001AFC10 call tail).
//
// WALL: register-coloring permutation -- the body is correct, residual is the shift
// base constant 1 living in $a3 (target) vs $a2 (here) threaded through the bit-table
// math, plus the state-2 beql branch-likely / func_001AFC10 tail block placement.
// Pure allocation/scheduling order -- permuter territory.
extern unsigned char D_00810700;
extern unsigned char D_00810841[];

extern void func_001AFC10(int self);
extern void func_001B0FD0(void);
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
            func_001B0FD0();
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
        func_001AFC10(s0);
        break;
    case 3:
        func_001AFC10(s0);
        break;
    }
}
