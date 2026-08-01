// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: per-frame ballistic/launch update on an entity (arg0) driven by a
//   sub-object (arg1). If any of the low 7 bits of the flag byte at arg1+0x80
//   are set, do nothing. Otherwise decay the vertical rate at arg1+0x48 by 0.1
//   per frame and fold it into the entity height arg0+0xB4. If the "landed"
//   byte at arg1+0x8C is set, stop there. Otherwise, if the 0x80 flag is clear,
//   the value at arg1+0x68 is <= 10.0 and the proximity/aim test
//   func_001B1560(self, &D_00810360, 1.13446f) passes, OR bit 0 into the entity
//   status word at arg0+0x52 and stop. Failing that, advance the horizontal
//   speed arg1+0x4C by its acceleration arg1+0x50 and step the entity position
//   (arg0+0xB0 / arg0+0xB8) along the yaw at arg0+0xC4 using cos/sin
//   (func_0011E2A8 / func_0011DE90).
//
//   MATCHING DEVICES (both semantic no-ops, see docs/fanout/MATCHING_GUIDE.md):
//   the split `t = *p; t += -0.1f; *p = t;` and the compound `*p += *q;` steer
//   mwcc's add.s operand order to the target's (idiom-21/26). The fused
//   `t = *p + -0.1f;` / `*p = *p + *q;` spellings emit the operands swapped.

extern int func_001B1560(char *a0, char *a1, float f);
extern float func_0011E2A8(float);
extern float func_0011DE90(float);
extern char D_00810360;

void func_0014AE80(char *arg0, char *arg1) {
    float t;

    if (*(char *)(arg1 + 0x80) & 0x7F) {
        return;
    }
    t = *(float *)(arg1 + 0x48);
    t += -0.1f;
    *(float *)(arg1 + 0x48) = t;
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + t;
    if (*(char *)(arg1 + 0x8C) != 0) {
        return;
    }
    if (!(*(char *)(arg1 + 0x80) & 0x80) && *(float *)(arg1 + 0x68) <= 10.0f
            && func_001B1560(arg0, &D_00810360, 1.1344640254974365f) != 0) {
        *(unsigned short *)(arg0 + 0x52) = *(unsigned short *)(arg0 + 0x52) | 1;
        return;
    }
    *(float *)(arg1 + 0x4C) += *(float *)(arg1 + 0x50);
    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0)
            + *(float *)(arg1 + 0x4C) * func_0011E2A8(*(float *)(arg0 + 0xC4));
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8)
            + *(float *)(arg1 + 0x4C) * func_0011DE90(*(float *)(arg0 + 0xC4));
}
