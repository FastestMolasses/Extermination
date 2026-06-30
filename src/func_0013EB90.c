// NEARMISS func_0013EB90  (vram 0x0013EB90, 0x214 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.26% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling permutation (NOT clean-store delay-slot nop). Logic + structure fully recovered and byte-correct (all stores/calls/constants/control-flow match). Residual: target keeps state byte (p+6) in volatile $a1 (p->$s1, e->$s0, 0x30 frame); mwcc keeps it in callee-saved $s0 ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS (92.26% mwcc 2.3.3; 80.01% mwcc 991202). Logic fully recovered;
// structure (control flow, all stores, calls, constants) is byte-correct. Sole
// residual is a register-allocation/scheduling permutation: the target keeps the
// state byte (p+6) in volatile $a1 with p->$s1, e->$s0 and a 0x30 frame, whereas
// mwcc keeps it in callee-saved $s0 to satisfy the final `return state+1`,
// spilling an extra $s2 (p->$s2, e->$s1, 0x40 frame) plus a branch-likely-sense
// diff (target beqzl vs our beqz). Permuter territory, not a clean-store nop wall.
//
// Animation-state step for the player/entity keyed on the state byte at p+6.
// Always sets e+0x85 = 1 first. State 0: advance to 1, seed e+0x40 = 1.0f,
// e+0x81 = 0, e+0x50 = 0.02f, clear high bits of e+0x86, kick anim_clip_init(p, 7,
// 5.0f, 0.0f). State 1: clamp the e+0x4C timer to 0.6f (and zero e+0x50 when
// clamped); if e+0x86 == 1, mark it |=2 and run the scratch-matrix setup at
// 0x700038A0 (col offsets 0/4/8/C = 0,0,5.0f,1.0f) feeding func_001B2B10 /
// func_001028B8 (p+0xB0) / func_001EFD90 (p+0xC0) / func_001E8B90; then when the
// e+0x48 timer has expired (<=0) and e+0x80 bit 0x2 is set, advance p+6, zero the
// e+0x48/0x4C/0x50 timers, set e+0x81 = 1, fire event 0x7F9 at 300.0f and kick
// anim_clip_init(p, 8, 0.0f, 0.0f). State 2: when e+0x30 bit 0x1000 is set, set
// p+5 = 4, p+6 = 0, e+0x81 = 0. Returns state+1 (only the default/>=3 path's
// return value is observed by callers).
extern void anim_clip_init(void *p, int n, float a, float b);
extern int func_001B2B10(char *, void *, void *);
extern int func_001028B8(void *, void *, void *);
extern int func_001EFD90(int, void *, void *);
extern int func_001E8B90(void *, float);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char D_700038A0[];

int func_0013EB90(char *p, char *e)
{
  unsigned char state;

  *(unsigned char *)(e + 0x85) = 1;
  state = *(unsigned char *)(p + 6);

  switch (state)
  {
    case 0:
      *(char *)(p + 6) = state + 1;
      *(float *)(e + 0x40) = 1.0f;
      *(char *)(e + 0x81) = 0;
      *(float *)(e + 0x50) = 0.02f;
      *(char *)(e + 0x86) = (char)(*(char *)(e + 0x86) & 1);
      anim_clip_init(p, 7, 5.0f, 0.0f);
      break;

    case 1:
      if (*(float *)(e + 0x4C) > 0.6f)
      {
        *(float *)(e + 0x4C) = 0.6f;
        *(int *)(e + 0x50) = 0;
      }
      if (*(char *)(e + 0x86) == 1)
      {
        *(char *)(e + 0x86) = (char)(*(char *)(e + 0x86) | 2);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(float *)0x700038A8 = 5.0f;
        *(float *)0x700038AC = 1.0f;
        func_001B2B10(p, &D_700038A0, &D_700038A0);
        func_001028B8(&D_700038A0, p + 0xB0, &D_700038A0);
        *(float *)0x700038A4 = *(float *)(e + 0x60);
        func_001EFD90(0x80000016, &D_700038A0, p + 0xC0);
        func_001E8B90(&D_700038A0, 5.0f);
      }
      if (*(float *)(e + 0x48) <= 0.0f)
      {
        if (*(char *)(e + 0x80) & 2)
        {
          *(char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
          *(int *)(e + 0x48) = 0;
          *(int *)(e + 0x4C) = 0;
          *(int *)(e + 0x50) = 0;
          *(char *)(e + 0x81) = 1;
          func_001FBD50(p, 0x7F9, 0, 300.0f);
          anim_clip_init(p, 8, 0.0f, 0.0f);
        }
      }
      break;

    case 2:
      if (*(int *)(e + 0x30) & 0x1000)
      {
        *(char *)(p + 5) = 4;
        *(char *)(p + 6) = 0;
        *(char *)(e + 0x81) = 0;
      }
      break;

    default:
      break;
  }
  return state + 1;
}
