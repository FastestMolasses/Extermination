// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Gated relative-offset setup against a global target object. Runs only when the mode byte
// D_00810700 == 2 and the global object pointer at 0x700031D4 is non-null with its type tag
// (+3) == 0x54 ('T'). For a self object whose +0 flag bit 0 is set: set state +0 = 2 and
// +0xF = 7, store the X/Z deltas of self minus target at self+0x70 / self+0x78 (with
// +0x74 = 0 and +0x7C = 1.0f), then normalize that vector in place via
// func_00102760(self+0x70, self+0x70). Returns 1 when the gate passed, else 0.

extern void func_00102760(void *a, void *b);
extern unsigned char D_00810700;

int func_001762E0(unsigned char *p)
{
  char *t;
  if (D_00810700 == 2)
  {
    t = *(char **)0x700031D4;
    if ((t != 0) && (*(unsigned char *)(t + 3) == 0x54))
    {
      if (p[0] & 1)
      {
        p[0] = 2;
        p[0xF] = 7;
        *(float *)(p + 0x70) = *(float *)(p + 0xB0) - *(float *)(t + 0xB0);
        *(int *)(p + 0x74) = 0;
        *(float *)(p + 0x78) = *(float *)(p + 0xB8) - *(float *)(t + 0xB8);
        *(int *)(p + 0x7C) = 0x3F800000;
        func_00102760(p + 0x70, p + 0x70);
      }
      return 1;
    }
  }
  return 0;
}
