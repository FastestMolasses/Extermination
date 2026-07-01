// NEARMISS func_0021FB40  (vram 0x0021FB40, 0x388 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Switch-dispatch branch-scheduling artifact: frame size (0x20), single callee-saved reg (s0), and every per-state body are byte-identical in structure and content -- confirmed via inverse-CSE reload-per-use idiom on repeated *D_00275B40 dereferences. The residual is that the target leaves 3 delay-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Turret/area-logic state-machine step, dispatched on the state byte at
// arg0+6 (0..4). State 0 arms the entity: fires an event via
// func_001FBD50(arg0, 0x154, 0, 300.0f), advances the state byte, resets
// the sub-state byte arg0+7, re-arms the clip via func_001749A0(arg0,
// 0x2F, 0), zeroes 0x21C/0x38/0x2E4, and copies the base transform
// arg0+0xB0 into arg0+0x290 via func_00102948. State 1 stages the
// tracked target's transform (*D_00275B40) into D_700038A0/A4/A8 (also
// caching Y into arg0+0x2E4), builds a lead position via
// func_001026A0(&D_700038B0, arg0+0xD0, &D_700038A0) (matrix*vector) and
// func_001028B8(arg0+0xB0, arg0+0x290, &D_700038B0) (vector add),
// re-copies arg0+0x290/0xB0 into D_700038A0/D_700038B0, biases both
// D_700038B4 and D_700038A4 by +10.0f, and probes with
// func_0019AFE0(arg0, &D_700038A0, &D_700038B0, 6); on a hit, nudges
// arg0+0xB0/0xB8 by 4.5x the shared target pointer's 0x24/0x2C offsets.
// If the area-active bit (arg0+0x200 & 0x1000) is set, advances to state
// 2, re-arms the clip (0x30), and resets 0x28/0x2FE/0x26C. State 2
// counts down the 0x2FE cooldown byte; once func_001754E0(arg0,0xA)
// trips (or the shared abort flag 0x70003B8D is set), it advances to
// state 3, sets the global alert latch D_008106BD, resets 0x224/0x38/
// 0x21C; otherwise it just republishes 0x26C from 0x24C. State 3 checks
// the area-active bit again: if set, advances to state 4 and zeroes
// 0x2EC; otherwise it accumulates the tracked target's Y/vertical delta
// into 0x38/0x2EC (against the cached 0x21C/0x2E4), republishes via
// func_00178B90(1), and bumps arg0+0xB4 by the delta before calling
// func_00175900(arg0,1). State 4 decays the settle spring at arg0+0x2EC
// (func_00179880) and, once func_00175900(arg0,1) reports settled,
// re-arms the 0x20E timer and clears 0x25C via func_0017C540.
extern void func_00102948(void *dst, void *src);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001026A0(void *dst, void *matrix, void *vec);
extern int func_001749A0(int a0, short a1, int a2);
extern void func_00174AC0(void *a0, int a1);
extern int func_001754E0(void *a0, int a1);
extern int func_00175900(void *a0, int a1);
extern void func_00178B90(int a0);
extern void func_00179880(unsigned char *a0, float *a1);
extern void func_0017C540(unsigned char *a0);
extern int func_0019AFE0(void *p, void *a, void *b, int n);
extern void func_001FBD50(void *a0, int a1, int a2, float f12);

extern void **D_00275B40;
extern unsigned char D_008106BD;
extern float D_700038A0;
extern float D_700038A4;
extern float D_700038A8;
extern int D_700038AC;
extern float D_700038B0;
extern float D_700038B4;
extern unsigned char D_70003B8D;

void func_0021FB40(unsigned char *arg0) {
    switch (arg0[6]) {
    case 0:
        func_001FBD50(arg0, 0x154, 0, 300.0f);
        arg0[6]++;
        arg0[7] = 0;
        func_001749A0((int)arg0, 0x2F, 0);
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x38) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        func_00102948(arg0 + 0x290, arg0 + 0xB0);
        return;

    case 1: {
        D_700038A0 = *(float *)*D_00275B40;
        D_700038A4 = *(float *)((char *)*D_00275B40 + 4);
        *(float *)(arg0 + 0x2E4) = D_700038A4;
        D_700038A8 = *(float *)((char *)*D_00275B40 + 8);
        D_700038AC = 0;
        func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);
        func_001028B8(arg0 + 0xB0, arg0 + 0x290, &D_700038B0);
        func_00102948(&D_700038A0, arg0 + 0x290);
        func_00102948(&D_700038B0, arg0 + 0xB0);
        D_700038B4 = 10.0f + D_700038B4;
        D_700038A4 = D_700038B4;
        if (func_0019AFE0(arg0, &D_700038A0, &D_700038B0, 6) != 0) {
            *(float *)(arg0 + 0xB0) = *(float *)0x700031B0 + (4.5f * *(float *)((char *)(*(void **)0x700031D0) + 0x24));
            *(float *)(arg0 + 0xB8) = *(float *)0x700031B8 + (4.5f * *(float *)((char *)(*(void **)0x700031D0) + 0x2C));
        }
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6]++;
            func_001749A0((int)arg0, 0x30, 0);
            *(short *)(arg0 + 0x28) = 0;
            arg0[0x2FE] = 0;
            func_00174AC0(arg0, 0);
            *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
        }
        return;
    }

    case 2: {
        signed char cd = arg0[0x2FE];
        arg0[0x2FE] = cd - 1;
        if (cd == 0) {
            arg0[0x2FE] = 0;
            *(short *)(arg0 + 0x28) = 0;
        }
        func_00174AC0(arg0, 0);
        if (func_001754E0(arg0, 0xA) != 0 || D_70003B8D != 0) {
            arg0[6]++;
            D_008106BD = 1;
            *(int *)(arg0 + 0x224) = 0;
            func_001749A0((int)arg0, 0x31, 0);
            *(float *)(arg0 + 0x38) = 0.0f;
            *(float *)(arg0 + 0x21C) = 0.0f;
            return;
        }
        *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
        return;
    }

    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6]++;
            *(float *)(arg0 + 0x2EC) = 0.0f;
            return;
        }
        *(float *)(arg0 + 0x38) = ((float *)*D_00275B40)[2] - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = ((float *)*D_00275B40)[2];
        func_00178B90(1);
        *(float *)(arg0 + 0x2EC) = ((float *)*D_00275B40)[1] - *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0x2E4) = ((float *)*D_00275B40)[1];
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)(arg0 + 0x2EC);
        func_00175900(arg0, 1);
        return;

    case 4:
        func_00179880(arg0, (float *)(arg0 + 0x2EC));
        if (func_00175900(arg0, 1) != 0) {
            *(short *)(arg0 + 0x20E) = 0x3C;
            arg0[0x25C] = 0;
            func_0017C540(arg0);
        }
        break;
    }
}
