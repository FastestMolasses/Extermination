// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003B68 0x700038A0 0x700038A4 0x700038A8 0x700038AC 0x700038B0 0x700038B4 0x700038B8 0x700038BC

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.56% NEARMISS).
// Fixes read from the original instructions:
//  - The hardware counter word 0x70003B68 is a relocated extern (idiom-32); as a
//    plain literal mwcc speculated its upper half into the probe branch's delay
//    slot, which the original leaves empty.
//  - The second probe block (0x700038A0..BC, both arms) is written through
//    volatile relocated externs, one per word (opted in with // SPAD:); the
//    float compare's delay slot then stays empty and the +0.003f nudge reloads
//    and stores 0x700038A4 the way the original does. The first two staging
//    blocks stay literal stores.
//  - The state value (2 or 4) is assigned at the top of each arm, which puts
//    its set-up ahead of the last staging stores as in the original.
//
// Sets up a probe transform in the scratchpad block at 0x700038A0..AC, then runs
// func_001B2B10/func_001028B8/func_0019AD00 against arg0's forward direction
// (arg0+0xB0) with mask 0x80000006. A hit ORs bit 1 into *(arg1+0x81); on a miss
// it retries the same trio with a sideways offset of +6.0f or -6.0f chosen by the
// low bit of the counter at 0x70003B68, ORing bit 1 again on a hit. Then, keyed on
// whether *(arg1+0x50) <= 0.0f, it stages a second probe block (state 2: -4.0f and
// -8.0f; state 4: +2.0f and +4.0f; w = 1.0f) and runs func_001028B8 to transform it
// by arg0+0xB0; the state-4 arm also nudges 0x700038A4 by +0.003f. Finally it probes
// with func_0019AB20 (mask 0x80000007, second point D_700038B0) and, on a hit, ORs
// the selected state into *(arg1+0x81).
extern void func_001B2B10(void *p, void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019AD00(void *p, void *a, int b);
extern int func_0019AB20(void *p, void *a, void *b, int c);
extern volatile int D_700038A0;
extern volatile float D_700038A4;
extern volatile int D_700038A8;
extern volatile int D_700038AC;
extern volatile int D_700038B0;
extern volatile int D_700038B4;
extern volatile int D_700038B8;
extern volatile int D_700038BC;
extern int D_70003B68;

void func_0013BF20(char *arg0, char *arg1)
{
  int st;

  *(char *)(arg1 + 0x81) = 0;
  *(int *)0x700038A0 = 0;
  *(int *)0x700038A4 = 0;
  *(int *)0x700038A8 = 0x40400000;
  *(int *)0x700038AC = 0x3F800000;
  func_001B2B10(arg0, &D_700038A0, &D_700038A0);
  func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
  if (func_0019AD00(arg0, &D_700038A0, 0x80000006) != 0) {
    *(char *)(arg1 + 0x81) = *(char *)(arg1 + 0x81) | 1;
  } else {
    if (D_70003B68 & 1) {
      *(int *)0x700038A0 = 0x40C00000;
      *(int *)0x700038A4 = 0;
      *(int *)0x700038A8 = 0;
      *(int *)0x700038AC = 0x3F800000;
    } else {
      *(int *)0x700038A0 = 0xC0C00000;
      *(int *)0x700038A4 = 0;
      *(int *)0x700038A8 = 0;
      *(int *)0x700038AC = 0x3F800000;
    }
    func_001B2B10(arg0, &D_700038A0, &D_700038A0);
    func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
    if (func_0019AD00(arg0, &D_700038A0, 0x80000006) != 0) {
      *(char *)(arg1 + 0x81) = *(char *)(arg1 + 0x81) | 1;
    }
  }
  if (*(float *)(arg1 + 0x50) <= 0.0f) {
    st = 2;
    D_700038A0 = 0;
    D_700038B0 = 0;
    D_700038A4 = -4.0f;
    D_700038A8 = 0;
    D_700038AC = 0x3F800000;
    D_700038B4 = 0xC1000000;
    D_700038B8 = 0;
    D_700038BC = 0x3F800000;
    func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
  } else {
    st = 4;
    D_700038A0 = 0;
    D_700038B0 = 0;
    D_700038A4 = 2.0f;
    D_700038A8 = 0;
    D_700038AC = 0x3F800000;
    D_700038B4 = 0x40800000;
    D_700038B8 = 0;
    D_700038BC = 0x3F800000;
    func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
    D_700038A4 += 0.003f;
  }
  if (func_0019AB20(arg0, &D_700038A0, &D_700038B0, 0x80000007) != 0) {
    *(char *)(arg1 + 0x81) = *(char *)(arg1 + 0x81) | st;
  }
}
