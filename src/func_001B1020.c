// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Binds a model to an actor and initialises its bones. func_001B0DC0(actor,
// model, clip) does the bind; on failure (non-zero) this returns 1. Otherwise
// the actor's bind counter byte +4 is incremented and the bones get their rest
// pose: bone_init_default_1(actor) when clip == -1, else
// bone_init_default_2(actor, frame) with the clip's start frame. Returns 0.
extern int func_001B0DC0(char *actor, int model, int clip);
extern void bone_init_default_1(char *actor);
extern void bone_init_default_2(char *actor, short frame);

int func_001B1020(char *actor, int model, int clip, int frame) {
    if (func_001B0DC0(actor, model, clip) != 0) {
        return 1;
    }
    ((unsigned char *)actor)[4]++;
    if (clip == -1) {
        bone_init_default_1(actor);
    } else {
        bone_init_default_2(actor, frame);
    }
    return 0;
}
