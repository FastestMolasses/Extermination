// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player state 8 (see func_0015B530): dispatches on the sub-state byte +6.
// 0 does nothing; 1 calls func_0021D2E0(actor, 0x78, 0); 2..5 and 0x0A run
// func_00164220, func_00163E90, func_00163D50, func_00163C10 and
// func_001643B0 on the actor.
extern void func_0021D2E0(char *actor, int a, int b);
extern void func_00164220(char *actor);
extern void func_00163E90(char *actor);
extern void func_00163D50(char *actor);
extern void func_00163C10(char *actor);
extern void func_001643B0(char *actor);

void func_00163B40(char *actor) {
    switch ((unsigned char)actor[6]) {
    case 0:
        break;
    case 1:
        func_0021D2E0(actor, 0x78, 0);
        break;
    case 2:
        func_00164220(actor);
        break;
    case 3:
        func_00163E90(actor);
        break;
    case 4:
        func_00163D50(actor);
        break;
    case 5:
        func_00163C10(actor);
        break;
    case 0x0A:
        func_001643B0(actor);
        break;
    }
}
