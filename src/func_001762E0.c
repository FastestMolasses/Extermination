// NEARMISS func_001762E0  (vram 0x001762E0, 0xAC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling / pointer-copy-direction artifact. Full logic recovered (hoisting t load inside the D_00810700==2 guard and collapsing the duplicate return-0 took it 68%->94.65%; mode-byte gate, type-tag 0x54 check, flag-bit-0 gate, X/Z delta stores and the func_00102760 call all byte-iden...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Gated relative-offset setup against a global target object. Runs only when the
// mode byte D_00810700 == 2 and the global object pointer at 0x700031D4 is
// non-null with its type tag (+3) == 0x54 ('T'). For a self object whose +0 flag
// bit 0 is set: set state +0 = 2 and +0xF = 7, then store the X/Z deltas of self
// minus target at self+0x70 / self+0x78 (with +0x74 = 0 and +0x7C = 1.0f),
// and hand the resulting +0x70 vector to func_00102760(self+0x70, self+0x70,
// target). Returns 1 when the gate passed, else 0.
extern void func_00102760(float *a, float *b, char *c);
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
        func_00102760((float *)(p + 0x70), (float *)(p + 0x70), t);
      }
      return 1;
    }
  }
  return 0;
}
