// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-frame update of an animated object (installed as the +0x10 callback by
// func_0016BAE0). Phase byte at +4:
//   0: run the setup step func_00188430(self); when it returns 0 (success)
//      advance to phase 1.
//   1: advance the animation by one step (anim_advance_time(self, 1.0)) and
//      keep its result as the short status at +0x2E; run the per-type state
//      step (type byte +0xD: 0 -> func_00188180, 1 -> func_00188250, others
//      none); then call the object's own function pointer at +0x4C with self.
//   2, 3: func_001AFC10(self) (teardown/release).
// Other phases do nothing.

typedef struct Obj Obj;
struct Obj {
    unsigned char pad0[4];
    unsigned char phase;            /* +0x04 */
    unsigned char pad5[0xD - 5];
    unsigned char type;             /* +0x0D */
    unsigned char padE[0x2E - 0xE];
    short animStatus;               /* +0x2E (bit 0x1000 tested by the state steps) */
    unsigned char pad30[0x4C - 0x30];
    void (*update)(Obj *self);      /* +0x4C */
};

extern int func_00188430(Obj *self);
extern int anim_advance_time(Obj *self, float dt);
extern void func_00188180(Obj *self);
extern void func_00188250(Obj *self);
extern void func_001AFC10(Obj *self);

void func_00188340(Obj *self) {
    switch (self->phase) {
    case 0:
        if (func_00188430(self) == 0) {
            self->phase = 1;
        }
        break;
    case 1:
        self->animStatus = anim_advance_time(self, 1.0f);
        switch (self->type) {
        case 0:
            func_00188180(self);
            break;
        case 1:
            func_00188250(self);
            break;
        }
        self->update(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
