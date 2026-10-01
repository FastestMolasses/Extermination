// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player state step for one actor. While the scratchpad flag 0x70003B8D is
// clear the actor runs func_00182DF0 only. Otherwise its state byte +5 picks
// the state's worker: 0 -> func_001837A0, 0x17 -> func_00183910,
// 1 -> func_001837B0, 5 -> func_00162DB0 (the fall), 8 -> func_00163B40,
// 0x0C -> func_001838B0; any other state does nothing.
extern unsigned char D_70003B8D;
extern void func_00182DF0(char *actor);
extern void func_001837A0(char *actor);
extern void func_00183910(char *actor);
extern void func_001837B0(char *actor);
extern void func_00162DB0(char *actor);
extern void func_00163B40(char *actor);
extern void func_001838B0(char *actor);

void func_0015B530(char *actor) {
    if (D_70003B8D == 0) {
        func_00182DF0(actor);
        return;
    }
    switch ((unsigned char)actor[5]) {
    case 0:
        func_001837A0(actor);
        break;
    case 0x17:
        func_00183910(actor);
        break;
    case 1:
        func_001837B0(actor);
        break;
    case 5:
        func_00162DB0(actor);
        break;
    case 8:
        func_00163B40(actor);
        break;
    case 0x0C:
        func_001838B0(actor);
        break;
    }
}
