// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: 6-step scripted "glide to waypoint A, then to waypoint B" actor
// state machine, dispatched through a 6-entry jump table on the state byte.
//   e[+0x04]  = actor enable/ready flag
//   e[+0x05]  = secondary flag
//   e[+0x06]  = state (0..5); out-of-range does nothing
//   e[+0x07]  = per-state scratch flag
//   e[+0x28]  = short countdown timer, in frames
//   e[+0xB0], +0xB4, +0xB8   = position vec3 (x,y,z)
//   e[+0x200] = 32-bit flag word (bit 0x1000 gates the step-4 -> 5 transition)
//   e[+0x2E0], +0x2E4, +0x2E8 = per-frame position delta vec3
//   e[+0x2EC] = float parameter handed to func_00179880 (-0.2f)
//   e[+0x1F0] = cleared on teardown
//   D_002487B0/B4/B8 = waypoint A vec3, D_002487C0/C4/C8 = waypoint B vec3
// step 0: play animation 0x180 at speed 8.0, arm a 120-frame glide and set the
//         per-frame delta so the actor reaches waypoint A in 120 frames.
// step 1: glide (pos += delta) until the timer expires, then snap to waypoint A
//         and arm a 30-frame glide toward waypoint B.
// step 2: glide until the timer expires, then snap to waypoint B.
// step 3: play animation 0x181 at speed 4.0 and return (no further work).
// step 4: wait for flag 0x1000 in +0x200, then set +0x2EC = -0.2f (stored as the
//         raw bit pattern 0xBE4CCCCD, which is how the original emits it).
// step 5: run func_00179880 with &+0x2EC each frame; once func_00175900 reports
//         completion, clear the shared byte at 0x70003B8D and reset the actor.
// The step-0 state bump is read through a volatile lvalue so that the reload of
// e[6] is not folded into the value the switch already holds in a register --
// that is what the original does here (steps 3 and 4 reuse the switch value).

extern void func_001749A0(unsigned char *, int, int, float);
extern void func_0016BAE0(unsigned char *, int);
extern void func_00179880(unsigned char *, float *);
extern int func_00175900(unsigned char *, int);

extern float D_002487B0;
extern float D_002487B4;
extern float D_002487B8;
extern float D_002487C0;
extern float D_002487C4;
extern float D_002487C8;

void func_001834E0(unsigned char *e)
{
    short t;
    int n;

    switch (e[6]) {
    case 0:
        n = *(volatile unsigned char *)(e + 6);
        e[6] = n + 1;
        e[7] = 0;
        func_001749A0(e, 0x180, 0, 8.0f);
        func_0016BAE0(e, 1);
        *(short *)(e + 0x28) = 0x78;
        *(float *)(e + 0x2E0) = (D_002487B0 - *(float *)(e + 0xB0)) / 120.0f;
        *(float *)(e + 0x2E4) = (D_002487B4 - *(float *)(e + 0xB4)) / 120.0f;
        *(float *)(e + 0x2E8) = (D_002487B8 - *(float *)(e + 0xB8)) / 120.0f;
        break;
    case 1:
        t = *(short *)(e + 0x28);
        *(short *)(e + 0x28) = t - 1;
        if (t == 0) {
            e[6] = e[6] + 1;
            *(float *)(e + 0xB0) = D_002487B0;
            *(float *)(e + 0xB4) = D_002487B4;
            *(float *)(e + 0xB8) = D_002487B8;
            *(short *)(e + 0x28) = 0x1E;
            *(float *)(e + 0x2E0) = (D_002487C0 - *(float *)(e + 0xB0)) / 30.0f;
            *(float *)(e + 0x2E4) = (D_002487C4 - *(float *)(e + 0xB4)) / 30.0f;
            *(float *)(e + 0x2E8) = (D_002487C8 - *(float *)(e + 0xB8)) / 30.0f;
        } else {
            *(float *)(e + 0xB0) += *(float *)(e + 0x2E0);
            *(float *)(e + 0xB4) += *(float *)(e + 0x2E4);
            *(float *)(e + 0xB8) += *(float *)(e + 0x2E8);
        }
        break;
    case 2:
        t = *(short *)(e + 0x28);
        *(short *)(e + 0x28) = t - 1;
        if (t == 0) {
            e[6] = e[6] + 1;
            *(float *)(e + 0xB0) = D_002487C0;
            *(float *)(e + 0xB4) = D_002487C4;
            *(float *)(e + 0xB8) = D_002487C8;
        } else {
            *(float *)(e + 0xB0) += *(float *)(e + 0x2E0);
            *(float *)(e + 0xB4) += *(float *)(e + 0x2E4);
            *(float *)(e + 0xB8) += *(float *)(e + 0x2E8);
        }
        break;
    case 3:
        e[6] = e[6] + 1;
        func_001749A0(e, 0x181, 0, 4.0f);
        return;
    case 4:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = e[6] + 1;
            *(int *)(e + 0x2EC) = 0xBE4CCCCD;
        }
        break;
    case 5:
        func_00179880(e, (float *)(e + 0x2EC));
        if (func_00175900(e, 0) != 0) {
            *(volatile char *)0x70003B8D = 0;
            e[4] = 1;
            e[5] = 0;
            e[6] = 0;
            e[0x1F0] = 0;
        }
        break;
    }
}
