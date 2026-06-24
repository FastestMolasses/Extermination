// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// State dispatch on the actor's state byte (arg0+4). State 0: if the bit
// (1 << *(arg0+0x2E)) is set in the byte-table D_00810841[D_00810700], force
// state=3; else if func_001B0FD0(self) returns 0, run setup
// func_001C6380(self) / mark byte+0 = 1 / func_001F1110(self, 2). State 1:
// when the halfword at +0x36 is set, mark +0/+4 = 2, OR the same table bit in,
// set D_00810845 |= 4, and fire func_001FB9F0(0x3F1,0x1000,0x1000,0x1000); then
// always tick func_001F1180/func_001B17A0(self) and dispatch the virtual at
// +0x4C. States 2 and 3: func_001AFC10(self).
//
// Matched with mwcc 2.3.3 (991202 reaches 80.9%). Three keys: (1) func_001B0FD0
// and func_001F1180 both take the self pointer, so a0 stays live as `self` and
// mwcc keeps the pointer in a0 / colors the state byte into a1 like the target
// (the `paddub s0,a0` self-save then rides the case-3 dispatch delay slot, and
// the case-1 `beqz` slot fills with the func_001B17A0 arg `paddub a0,s0`);
// (2) `case 2: case 3:` (ascending) reproduces the target's 3,2,1,0 descending
// compare chain with the `beql` on the case-2 test; (3) the byte-table is plain
// data (sdatathreshold 0 -> lui/%hi). objdiff 100.0 vs build/expected.
extern void func_001AFC10(char *p);
extern int func_001B0FD0(char *p);
extern void func_001C6380(char *p);
extern void func_001F1110(char *p, int a);
extern void func_001F1180(char *p);
extern void func_001B17A0(char *p);
extern int func_001FB9F0(int a, int b, int c, int d);
extern unsigned char D_00810700;
extern unsigned char D_00810841;
extern unsigned char D_00810845;

void func_00158430(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        if ((&D_00810841)[D_00810700] & ((1 << *(unsigned short *)(arg0 + 0x2E)) & 0xFF)) {
            *(unsigned char *)(arg0 + 4) = 3;
        } else if (func_001B0FD0(arg0) == 0) {
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
            D_00810845 |= 4;
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
