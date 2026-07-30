// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Enemy state machine tick: damage/death dispatch (sub-state 0), spawn-anim wait (1),
// knock-back ballistic slide with collision probe (2), despawn hand-off (3), then the
// common per-frame animation advance + post-update callback.
// The `zi = 0; ... z = (float)zi;` staging is load-bearing: it is what makes mwcc
// schedule `mtc1 zero,$f13` ahead of `mtc1 $v0,$f12` at the case-1 anim_clip_init call.
extern void func_001FBD50(void *p, int a, int b, float f);
extern void anim_clip_init(void *p, int clip, float a, float b);
extern int anim_advance_time(void *p, float t);
extern void func_00131ED0(void *p);
extern void func_001C68C0(void *p);
extern int func_001B17A0(void *p);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019AB20(void *self, void *b, void *c, int id);
extern int func_001EFE00(int a, void *b);

typedef struct Actor153 {
    unsigned char ev00; char f01, f02, f03;
    unsigned char state04;
    unsigned char sub05;
    char pad06[0x2E];
    short hp34;
    short dmg36;
    char pad38[0x14];
    int (* volatile post4C)(struct Actor153 *);
    char pad50[0x60];
    float posB0, posB4, posB8;
    char padBC[8];
    float yawC4;
} Actor153;

typedef struct Ctx153 {
    int status00;
    float vy04, ay08, speed0C;
    unsigned char f10;
} Ctx153;

extern unsigned char D_008107FB;
extern int D_700038A0[];
extern int D_700038B0[];

void func_00153B50(Actor153 *self, Ctx153 *ctx, int state) {
    Ctx153 *c; Actor153 *s; int hp; int clip; int zi; float z;
    state = self->sub05; c = ctx; s = self;
    switch (state) {
    case 0:
        hp = s->hp34 - s->dmg36; s->hp34 = hp;
        if ((short)hp <= 0) {
            s->sub05 = 2; c->vy04 = 0;
            *(int *)&c->ay08 = 0xBC23D70A;
            *(int *)&c->speed0C = 0xBCF5C28F;
            D_008107FB = 2;
            func_001FBD50(self, 0x7D8, 0, 300.0f);
            clip = 0x34; anim_clip_init(s, clip, 10.0f, 0.0f);
        } else {
            s->sub05 += 1;
            c->speed0C = 0; c->ay08 = 0; c->vy04 = 0;
            func_001FBD50(self, 0x7D4, 0, 300.0f);
            clip = 0x37; anim_clip_init(s, clip, 10.0f, 0.0f);
        }
        break;
    case 1:
        if (c->status00 & 0x1000) {
            s->state04 = 1;
            s->sub05 = 0;
            zi = 0;
            s->ev00 = 1;
            s->dmg36 = 0;
            z = (float)zi;
            anim_clip_init(self, 0x33, 10.0f, z);
        }
        break;
    case 2:
        c->vy04 += c->ay08;
        if (c->vy04 < -5.0f) {
            *(int *)&c->vy04 = 0xC0A00000; c->ay08 = 0;
        }
        s->posB4 += c->vy04;
        s->posB0 += c->speed0C * func_0011E2A8(s->yawC4);
        s->posB8 += c->speed0C * func_0011DE90(s->yawC4);
        *(volatile int *)0x700038A0 = 0;
        *(volatile int *)0x700038A4 = 0xC0000000;
        *(volatile int *)0x700038A8 = 0;
        *(volatile int *)0x700038AC = 0x3F800000;
        func_001028B8(D_700038A0, D_700038A0, &s->posB0);
        *(volatile int *)0x700038B0 = 0;
        *(volatile float *)0x700038B4 = c->vy04;
        *(volatile int *)0x700038B8 = 0;
        *(volatile int *)0x700038BC = 0x3F800000;
        if (func_0019AB20(s, D_700038A0, D_700038B0, 0x80000007)) {
            s->sub05 += 1;
            func_001FBD50(s, 0x7D5, 0, 300.0f);
            clip = 0x36; anim_clip_init(s, clip, 10.0f, 0.0f);
        }
        break;
    case 3:
        if (c->status00 & 0x1000) {
            s->sub05 = state + 1; c->f10 = 0;
            func_001FBD50(self, 0x7D7, 0, 300.0f);
            if (func_001EFE00(0x8000001E, s) == 0) s->state04 = 3;
        }
        break;
    case 4: break;
    }
    c->status00 = anim_advance_time(s, 1.0f);
    func_00131ED0(s); func_001C68C0(s);
    if (func_001B17A0(s) != 0) s->post4C(s);
}
