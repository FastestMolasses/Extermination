// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: per-frame update for one enemy actor, called as (actor, ctl)
// where `ctl` is the actor's behaviour/AI record. func_001B2140 is the
// "should this actor be ticked at all" gate; everything below is skipped
// when it returns 0.
//   e[+0x00]  = actor update/priority class (3 when ctl[+0x72] is set, else 1)
//   e[+0x05]  = behaviour id (0..8) -- selects the movement handler through a
//               9-entry jump table; ids >= 9 skip the handler entirely
//   e[+0x0A]  = cleared each frame before func_00146CE0
//   e[+0x4C]  = per-actor virtual update hook, called last with the actor
//   e[+0x5D]  = collision/response mode: 0x81 when ctl[+0x78] == 0
//               (plus func_001B4810), otherwise 1
//   e[+0x5E]  = set to 1 (actor is live this frame)
//   e[+0x9F]  = copy of ctl[+0x78]
//   e[+0xB0]  = actor position vec3
//   ctl[+0x30] = current animation id (rewritten from anim_advance_time)
//   ctl[+0x40] = animation time step
//   ctl[+0x5C] = horizontal distance/angle to the tracked world point
//   ctl[+0x72] = "high priority" flag
//   ctl[+0x78] = contact/stance flag
//   ctl[+0x7A] = "handler wants the heavy path" flag; cleared each frame and
//                set by behaviours 3, 4 and 7
// The two scratchpad vectors D_700038A0 and D_700038B0 receive the actor
// position and the tracked world point D_00810350; both y components
// (+0x700038A4 / +0x700038B4) are zeroed so func_001B15D0 measures in the
// XZ plane only, and the result is cached in ctl[+0x5C].
// While the actor is playing animation 9 or 8 it fires one of five random
// voice cues (0x827.. or 0x82C..) through func_001FBD50 at range 300.0.
// Then the fixed per-frame chain runs: func_00146CE0, func_00145880,
// func_001459A0, anim_advance_time (result stored back as the animation id),
// func_00131ED0, func_001C68C0, func_001B17A0 and finally the e[+0x4C] hook.

extern int func_001B2140(unsigned char *);
extern void func_00102948(float *, float *);
extern float func_001B15D0(float *, float *);
extern void func_001471E0(unsigned char *, unsigned char *);
extern void func_00142330(unsigned char *, unsigned char *);
extern void func_001424C0(unsigned char *, unsigned char *);
extern void func_001429D0(unsigned char *, unsigned char *);
extern void func_001434C0(unsigned char *, unsigned char *);
extern void func_00143610(unsigned char *, unsigned char *);
extern void func_001437E0(unsigned char *, unsigned char *);
extern void func_00143AF0(unsigned char *, unsigned char *);
extern void func_00144C20(unsigned char *, unsigned char *);
extern void func_00144040(unsigned char *, unsigned char *);
extern void func_001B4810(unsigned char *);
extern int func_00122BB8(void);
extern void func_001FBD50(unsigned char *, int, int, float);
extern void func_00146CE0(unsigned char *, unsigned char *);
extern void func_00145880(unsigned char *, unsigned char *);
extern void func_001459A0(unsigned char *, unsigned char *);
extern int anim_advance_time(unsigned char *, float);
extern void func_00131ED0(unsigned char *);
extern void func_001C68C0(unsigned char *);
extern void func_001B17A0(unsigned char *);

extern float D_700038A0[];
extern float D_700038B0[];
extern float D_00810350[];

void func_00142070(unsigned char *e, unsigned char *d)
{
    if (func_001B2140(e) != 0) {
        d[0x7A] = 0;
        e[0x5E] = 1;
        func_00102948(D_700038A0, &((float *)e)[0x2C]);
        func_00102948(D_700038B0, D_00810350);
        *(volatile int *)0x700038B4 = 0;
        *(volatile int *)0x700038A4 = 0;
        *(float *)(d + 0x5C) = func_001B15D0(D_700038A0, D_700038B0);
        e[0x9F] = d[0x78];
        func_001471E0(e, d);
        switch (e[5]) {
        case 0:
            func_00142330(e, d);
            break;
        case 1:
            func_001424C0(e, d);
            break;
        case 2:
            func_001429D0(e, d);
            break;
        case 3:
            d[0x7A] = 1;
            func_001434C0(e, d);
            break;
        case 4:
            d[0x7A] = 1;
            func_00143610(e, d);
            break;
        case 5:
            func_001437E0(e, d);
            break;
        case 6:
            func_00143AF0(e, d);
            break;
        case 7:
            d[0x7A] = 1;
            func_00144C20(e, d);
            break;
        case 8:
            func_00144040(e, d);
            break;
        }
        if (d[0x72] != 0) {
            e[0] = 3;
        } else {
            e[0] = 1;
        }
        if (*(char *)(d + 0x78) == 0) {
            e[0x5D] = 0x81;
            func_001B4810(e);
        } else {
            e[0x5D] = 1;
        }
        if (d[0x30] == 9) {
            func_001FBD50(e, ((func_00122BB8() >> 13) % 5) + 0x827, 0, 300.0f);
        } else if (d[0x30] == 8) {
            func_001FBD50(e, ((func_00122BB8() >> 13) % 5) + 0x82C, 0, 300.0f);
        }
        e[0xA] = 0;
        func_00146CE0(e, d);
        func_00145880(e, d);
        func_001459A0(e, d);
        *(int *)(d + 0x30) = anim_advance_time(e, *(float *)(d + 0x40));
        func_00131ED0(e);
        func_001C68C0(e);
        func_001B17A0(e);
        (*(void (**)(unsigned char *))(e + 0x4C))(e);
    }
}
