// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// State machine on the byte at +4. State 0: set state=1, set the +0x20 float
// timer to 300.0f, then fall into state 1. State 1: run func_001028B8(self+0xB0,
// self+0xB0, self+0xC0), decrement the +0x20 timer by 1.0f; if it reaches <= 0
// advance state to 3; then func_001E0AC0(self+0xB0, &D_0026E9A0, 75.0f, 30.0f).
// States 2 and 3: func_001AFC10(self). Any other state: no-op.
//
// Match key: ascending case labels with the 0->1 fall-through and the shared
// 2/3 body, so mwcc emits the target's descending beql/beq/beql/beqz dispatch
// (cases 3 and 1 get branch-likely with filled delay slots). objdiff 100.0.
extern void func_001028B8(void *a, void *b, void *c);
extern void func_001E0AC0(void *p, void *tbl, float a, float b);
extern void func_001AFC10(void *p);
extern int D_0026E9A0;

void func_001E76A0(unsigned char *p)
{
  unsigned char state;
  float v;
  state = p[4];
  switch (state)
  {
    case 0:
      p[4] = 1;
      *(float *)(p + 0x20) = 300.0f;
      /* fall through */
    case 1:
      func_001028B8(p + 0xB0, p + 0xB0, p + 0xC0);
      v = *(float *)(p + 0x20) - 1.0f;
      *(float *)(p + 0x20) = v;
      if (v <= 0.0f)
      {
        p[4] = 3;
      }
      func_001E0AC0(p + 0xB0, &D_0026E9A0, 75.0f, 30.0f);
      break;
    case 2:
    case 3:
      func_001AFC10(p);
      break;
  }
}
