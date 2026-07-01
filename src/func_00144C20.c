// NEARMISS func_00144C20  (vram 0x00144C20, 0x488 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.27% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (enemy engage/disengage state machine, cases 0/1/2/3 on state byte arg0+6). Cracked the frame-size bloat (inverse-CSE'd the D_00275B40->[4] aim-field dereference at every use instead of caching -- matches target's re-derive-per-use shape) and a bad m2c-decode bug (c...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Per-state enemy behavior handler. arg0 is the actor, arg1 the paired
// control record. Dispatches on the state byte at arg0+6 (cases 0/1/2/3).
//   case 0: bump state, clear arg1+0x75, set arg1+0x40=1.0f, arg1+0x48=2.0f,
//     arg1+0x4C=1.3f, run func_001FBD50(self, 0x831, 0, 300.0f) and
//     anim_clip_init(self, 7, 0, 17.0f). Then steers arg0+0xC4 toward the
//     global heading D_00810374 (clamped to +-pi/4) via func_001B1470, and
//     sets arg1+0x44 to the +-pi offset from that heading.
//   case 1: once arg1+0x48 goes negative, bump state and dispatch
//     anim_clip_init(self, 9, 0, 0). Otherwise, if the arg0+0x2C flags
//     (masked 0xFFFF7FFF) aren't 8 and arg1+0x30 bit 0x1000 is set, dispatch
//     anim_clip_init(self, 8, 0, 0) (the flags value is tested only, not
//     passed -- target reuses the leftover $f12/$f13 zero regs for the call).
//   case 2: once arg0+0x3C<=31.0f, clear arg1+0x40. If arg1+0x74 bit 2 set,
//     bump state, arm the timers (arg1+0x40=1.0f, arg1+0x50=-0.05f,
//     arg1+0x54=10.0f, arg1+0x58=-0.275f), run func_001FBD50(self, 0x832, 0,
//     300.0f); if func_001B1560(self, D_00810360, 1.3962635f) fails, dispatch
//     anim_clip_init(self, 6, 5.0f, 12.0f); else reset the actor (arg0+5=2,
//     arg0+6=2, arg0+7=0; arg1+0x4C=1.15f; arg1+0x50=0) and dispatch
//     anim_clip_init(self, 5, 5.0f, 20.0f).
//   case 3: clear arg1+0x7A; clamp arg1+0x4C/0x50 and arg1+0x54/0x58 to >=0;
//     accumulate arg1+0x54 += arg1+0x58; steer the shared aim field at
//     D_00275B40->[4]+0x74 toward (pi*arg1+0x54)/180 via func_001B12B0(pi).
//     If that field reached pi (or arg1+0x54 hit 0), fold it into arg0+0xC4,
//     clear it and arg1+0x50/0x4C, and if arg1+0x5C<=20.0f run
//     func_001B1560(self, D_00810360, pi/4): success -> arg0+5=6, failure/
//     otherwise -> arg0+5=2; either way clear arg0+6.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern float func_001B1470(float a);
extern int func_001B1560(char *self, float *tbl, float f);
extern float func_001B12B0(float goal, float cur, float rate);
extern int D_00275B40;
extern float D_00810360[8];
extern float D_00810374[8];

void func_00144C20(char *arg0, char *arg1) {
  unsigned char st;

  st = *(unsigned char *)(arg0 + 6);
  switch (st) {
  case 0: {
    float d, a;

    *(unsigned char *)(arg0 + 6) = st + 1;
    *(char *)(arg1 + 0x75) = 0;
    *(float *)(arg1 + 0x40) = 1.0f;
    *(float *)(arg1 + 0x48) = 2.0f;
    *(float *)(arg1 + 0x4C) = 1.3f;
    func_001FBD50(arg0, 0x831, 0, 300.0f);
    anim_clip_init(arg0, 7, 0.0f, 17.0f);
    d = func_001B1470(D_00810374[0] - *(float *)(arg0 + 0xC4));
    *(float *)0x70003A20 = d;
    if (d < 0.0f) {
      if (d < -0.7853982f) {
        *(float *)0x70003A20 = -0.7853982f;
      }
      a = func_001B1470(*(float *)(arg0 + 0xC4) + *(float *)0x70003A20);
      *(float *)(arg0 + 0xC4) = a;
      *(float *)(arg1 + 0x44) = func_001B1470(a - 3.1241393f);
      return;
    }
    if (!(d <= 0.7853982f)) {
      *(float *)0x70003A20 = 0.7853982f;
    }
    a = func_001B1470(*(float *)(arg0 + 0xC4) + *(float *)0x70003A20);
    *(float *)(arg0 + 0xC4) = a;
    *(float *)(arg1 + 0x44) = func_001B1470(3.1241393f + a);
    return;
  }
  case 1:
    if (*(float *)(arg1 + 0x48) < 0.0f) {
      *(unsigned char *)(arg0 + 6) = st + 1;
      anim_clip_init(arg0, 9, 0.0f, 0.0f);
      return;
    }
    if ((*(short *)(arg0 + 0x2C) & 0xFFFF7FFF) != 8 && (*(int *)(arg1 + 0x30) & 0x1000)) {
      anim_clip_init(arg0, 8, 0.0f, 0.0f);
      return;
    }
    return;
  case 2:
    if (*(float *)(arg0 + 0x3C) <= 31.0f) {
      *(float *)(arg1 + 0x40) = 0.0f;
    }
    if (*(char *)(arg1 + 0x74) & 2) {
      *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
      *(float *)(arg1 + 0x40) = 1.0f;
      *(float *)(arg1 + 0x50) = -0.05f;
      *(float *)(arg1 + 0x54) = 10.0f;
      *(float *)(arg1 + 0x58) = -0.275f;
      func_001FBD50(arg0, 0x832, 0, 300.0f);
      if (func_001B1560(arg0, D_00810360, 1.3962635f) == 0) {
        anim_clip_init(arg0, 6, 5.0f, 12.0f);
        return;
      }
      *(char *)(arg0 + 5) = 2;
      *(unsigned char *)(arg0 + 6) = 2;
      *(char *)(arg0 + 7) = 0;
      *(float *)(arg1 + 0x4C) = 1.15f;
      *(int *)(arg1 + 0x50) = 0;
      anim_clip_init(arg0, 5, 5.0f, 20.0f);
      return;
    }
    break;
  case 3: {
    float ang;

    *(char *)(arg1 + 0x7A) = 0;
    if (*(float *)(arg1 + 0x4C) < 0.0f) {
      *(int *)(arg1 + 0x50) = 0;
      *(float *)(arg1 + 0x4C) = 0.0f;
    }
    if (*(float *)(arg1 + 0x54) < 0.0f) {
      *(float *)(arg1 + 0x58) = 0.0f;
      *(float *)(arg1 + 0x54) = 0.0f;
    }
    *(float *)(arg1 + 0x54) = *(float *)(arg1 + 0x54) + *(float *)(arg1 + 0x58);
    *(float *)(*(char **)(D_00275B40 + 4) + 0x74) = func_001B12B0(3.1415927f, *(float *)(*(char **)(D_00275B40 + 4) + 0x74), (3.1415927f * *(float *)(arg1 + 0x54)) / 180.0f);
    ang = *(float *)(*(char **)(D_00275B40 + 4) + 0x74);
    if (ang == 3.1415927f || *(float *)(arg1 + 0x54) == 0.0f) {
      *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4) + ang);
      *(int *)(*(char **)(D_00275B40 + 4) + 0x74) = 0;
      *(int *)(arg1 + 0x50) = 0;
      *(float *)(arg1 + 0x4C) = 0.0f;
      if (*(float *)(arg1 + 0x5C) <= 20.0f && func_001B1560(arg0, D_00810360, 0.7853982f) != 0) {
        *(char *)(arg0 + 5) = 6;
      } else {
        *(char *)(arg0 + 5) = 2;
      }
      *(unsigned char *)(arg0 + 6) = 0;
    }
    break;
  }
  }
}
