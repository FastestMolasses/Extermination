// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// State dispatcher keyed on the actor's state byte at +0x04. The sub-struct
// at +0x1F0 is passed as the second argument to each handler:
//   state 0 -> func_0015AC00(actor, &actor[0x1F0])
//   state 1 -> func_0015AE20(actor, &actor[0x1F0])
//   state 2,3 / default -> func_001B1190(actor[0x9A], &actor[0x1F0]);
//                          func_001AFC10(actor)
extern void func_0015AC00(void *actor, void *sub);
extern void func_0015AE20(void *actor, void *sub);
extern void func_001AFC10(void *actor);
extern void func_001B1190(int kind, void *sub);

void func_0015AFA0(char *actor) {
    void *sub;

    sub = actor + 0x1F0;
    switch (*(unsigned char *)(actor + 4)) {
    case 0:
        func_0015AC00(actor, sub);
        return;
    case 1:
        func_0015AE20(actor, sub);
        return;
    case 2:
    case 3:
    default:
        func_001B1190(*(unsigned char *)(actor + 0x9A), sub);
        func_001AFC10(actor);
        return;
    }
}
