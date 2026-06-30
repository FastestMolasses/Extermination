// NEARMISS func_0013B5B0  (vram 0x0013B5B0, 0x1DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.21% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// regalloc + scheduling near-miss (95.2% mwcc233): param/temp register coloring (target arg1->s0,arg0->s1,switch-temp->a1 vs mwcc arg0->s1,arg1->s0,switch-temp->a0); target duplicates the `addiu st+1` (beql slot + case entry) and leaves two branch delay slots as explicit nops that mwcc fills. Not t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 95.2% (mwcc 2.3.3; 991202=85.1%). Per-state animation step keyed on
// the state byte at arg0+6 (cases 0 and 1). Case 0: bump state, set arg1+0x44 =
// -1.8f and arg1+0x48 = 0x3D4CCCCD, run func_001FBD50(300.0f, 0x838, 0), then
// dispatch on (func_00122BB8() >> 17) & 3 -- nonzero kicks anim_clip_init(self,
// 0xF, 1.0f, 0.0f), zero kicks clip 0x10 -- and falls through. Case 1: clamp the
// arg1+0x44 timer to >=0 (zeroing +0x44/+0x48 when positive), and when it is <0
// stage the four words at 0x700038A0 (0,0,0xC0400000,0x3F800000) and run the
// func_001B2B10/func_001028B8/func_0019AD00 trio; finally, if arg1+0x70 has bit
// 0x1000 set, reset the actor (arg0[0]=1,[4]=1,[5]=2,[6]=0; arg1+0x44/+0x48=0;
// arg1+0x50=0xBFC6D3F2; clear arg1+0x20/+0x22; if arg0+0x36 & 0x5000 set
// arg1+0x34=0xA; clear arg0+0x36).
//
// Body + control flow fully recovered and instruction-sequence-identical; the
// residual is CW-vs-mwcc codegen, NOT the clean-store nop: (1) param/temp
// register coloring -- target colors arg1->s0, arg0->s1, switch temp->a1; mwcc
// picks arg0->s1, arg1->s0, switch temp->a0; (2) the case-0 dispatch `addiu
// v0,a1,1` (st+1) is emitted twice in the target (beql delay slot + case entry)
// where mwcc emits it once; (3) the target leaves several branch delay slots as
// explicit nops (after the +0x1000 beqz and the inner +0x5000 beqz) that mwcc
// fills. All three are register-allocation/instruction-scheduling artifacts ->
// permuter territory.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(float f, int a, int b);
extern int func_00122BB8(void);
extern void func_001B2B10(char *p, float *a, float *b);
extern void func_001028B8(float *a, float *b, char *c);
extern void func_0019AD00(char *p, float *a, int b);
extern float D_700038A0;

void func_0013B5B0(char *arg0, char *arg1)
{
  unsigned char st;
  float z;

  st = *(unsigned char *)(arg0 + 6);
  switch (st) {
  case 0:
    *(unsigned char *)(arg0 + 6) = st + 1;
    *(float *)(arg1 + 0x44) = -1.8f;
    *(int *)(arg1 + 0x48) = 0x3D4CCCCD;
    func_001FBD50(300.0f, 0x838, 0);
    if ((func_00122BB8() >> 0x11) & 3) {
      anim_clip_init(arg0, 0xF, 1.0f, z = 0.0f);
    } else {
      anim_clip_init(arg0, 0x10, 1.0f, z = 0.0f);
    }
  case 1:
    if (!(*(float *)(arg1 + 0x44) <= 0.0f)) {
      *(int *)(arg1 + 0x48) = 0;
      *(float *)(arg1 + 0x44) = 0.0f;
    }
    if (*(float *)(arg1 + 0x44) < 0.0f) {
      *(int *)0x700038A0 = 0;
      *(int *)0x700038A4 = 0;
      *(int *)0x700038A8 = 0xC0400000;
      *(int *)0x700038AC = 0x3F800000;
      func_001B2B10(arg0, &D_700038A0, &D_700038A0);
      func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
      func_0019AD00(arg0, &D_700038A0, 0x80000006);
    }
    if (*(int *)(arg1 + 0x70) & 0x1000) {
      *(char *)(arg0 + 0) = 1;
      *(char *)(arg0 + 4) = 1;
      *(char *)(arg0 + 5) = 2;
      *(unsigned char *)(arg0 + 6) = 0;
      *(int *)(arg1 + 0x48) = 0;
      *(float *)(arg1 + 0x44) = 0.0f;
      *(int *)(arg1 + 0x50) = 0xBFC6D3F2;
      *(short *)(arg1 + 0x22) = 0;
      *(short *)(arg1 + 0x20) = 0;
      if (*(short *)(arg0 + 0x36) & 0x5000) {
        *(short *)(arg1 + 0x34) = 0xA;
      }
      *(short *)(arg0 + 0x36) = 0;
    }
    return;
  }
}
