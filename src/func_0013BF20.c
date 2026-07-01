// NEARMISS func_0013BF20  (vram 0x0013BF20, 0x2CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// clean-store delay-slot-nop x2 + independent-instruction scheduling swap x2 (scheduler-internal, verified not source-order-driven)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS (mwcc 2.3.3 94.56%; pinned 991202 86.72%) -- logic fully recovered, readable
// plain C. Sets up a probe transform at the 0x700038Ax scratch block (D_700038A0..AC),
// runs func_001B2B10/func_001028B8/func_0019AD00 against arg0's forward direction
// (arg0+0xB0) with mask 0x80000006; on failure OR-in bit 1 to *(arg1+0x81). If that first
// probe succeeded, re-run the same trio with a transform picked by the low bit of the
// hardware counter at 0x70003B68 (+-0xC0400000 sideways offset) and OR bit 1 into
// *(arg1+0x81) again on failure. Then, keyed on whether *(arg1+0x50) <= 0.0f, stage a
// second probe block (0x700038Ax/Bx, states 2 vs 4 selecting +-4.0f/+2.0f pitch and
// -8.0f/4.0f yaw, always w=1.0f) and run func_001028B8 to build the probe transform at
// arg0+0xB0; the state==4 branch additionally nudges the resulting +0x38A4 float by
// +0.003f. Finally probe with func_0019AB20 (mask 0x80000007, dest D_700038B0) and, on a
// hit, OR the selected state (2 or 4) into *(arg1+0x81).
extern void func_001B2B10(void *p, void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019AD00(void *p, void *a, int b);
extern int func_0019AB20(void *p, void *a, void *b, int c);
extern float D_700038A0;
extern float D_700038B0;

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
    if (*(int *)0x70003B68 & 1) {
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
    *(int *)0x700038A0 = 0;
    *(int *)0x700038B0 = 0;
    *(int *)0x700038A4 = 0xC0800000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B4 = 0xC1000000;
    *(int *)0x700038B8 = 0;
    st = 2;
    *(int *)0x700038BC = 0x3F800000;
    func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
  } else {
    *(int *)0x700038A0 = 0;
    *(int *)0x700038B0 = 0;
    *(int *)0x700038A4 = 0x40000000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B4 = 0x40800000;
    *(int *)0x700038B8 = 0;
    st = 4;
    *(int *)0x700038BC = 0x3F800000;
    func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
    *(float *)0x700038A4 += 0.003f;
  }
  if (func_0019AB20(arg0, &D_700038A0, &D_700038B0, 0x80000007) != 0) {
    *(char *)(arg1 + 0x81) = *(char *)(arg1 + 0x81) | st;
  }
}
