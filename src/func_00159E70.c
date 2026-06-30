// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Actor per-state update dispatcher. Switches on the actor state byte at p[4]:
//   state 0: if func_001B0FD0(p) returns 0, build the placed-prop world matrix
//            (func_001C6380) and stamp p[0x28] = 0x29 when D_00810700[0]==4 else 0x2A.
//   state 1: sub-switch on the sub-state byte p[5]:
//            sub 0: if the gate table D_00810C64[(short)p[0x28]] is nonzero, advance
//                   the sub-state, write pi/2 (0x3FC90FDB) into +0x78 of the actor
//                   record at *(D_00275B40+4), set the bit (1<<p[0x2E]) in
//                   D_00810841[D_00810700[0]], and rebuild the matrix (func_001C6380).
//                   Then run the per-actor cleanup func_001B17A0(p).
//            sub 1: nothing extra.
//            In all sub cases finally invoke the actor's update vtable slot
//            (*(void(**)(void*))(p+0x4C))(p).
//   states 2/3: func_001AFC10(p) (free/despawn back to pool).
// 100.0 byte-identical on mwcc 2.3.3 (idiom-20: source cases written ascending so
// mwcc lowers to a descending beq/beql dispatch with a branch-likely on state 2).
extern int func_001B0FD0(void *);
extern void func_001C6380(void *);
extern void func_001B17A0(void *);
extern void func_001AFC10(void *);
extern char *D_00275B40;
extern unsigned char D_00810700[0x200];
extern unsigned char D_00810841[0x200];
extern unsigned char D_00810C64[0x200];

void func_00159E70(unsigned char *p) {
    switch (p[4]) {
    case 0:
        if (func_001B0FD0(p) == 0) {
            func_001C6380(p);
            if (D_00810700[0] == 4) {
                *(short *)(p + 0x28) = 0x29;
            } else {
                *(short *)(p + 0x28) = 0x2A;
            }
        }
        break;
    case 1:
        switch (p[5]) {
        case 0:
            if (D_00810C64[*(short *)(p + 0x28)] != 0) {
                p[5] = p[5] + 1;
                *(int *)(*(char **)(D_00275B40 + 4) + 0x78) = 0x3FC90FDB;
                D_00810841[D_00810700[0]] |= (1 << p[0x2E]);
                func_001C6380(p);
            }
            func_001B17A0(p);
            break;
        case 1:
            break;
        }
        (*(void (**)(void *))(p + 0x4C))(p);
        break;
    case 2:
    case 3:
        func_001AFC10(p);
        break;
    }
}
