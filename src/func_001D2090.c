// INCLUDE_ASM func_001D2090  (vram 0x001D2090, 124 bytes)
// PARKED — register-allocation-ORDER wall, best 90.3% (analysis 2026-06-18, s81).
//
// Byte-identical machine code is supplied by fill_unmatched.py from the local
// splat .s. Do NOT make this a compiled unit until it reaches 100% — its
// build/obj/.o has the right SIZE (124 B) but wrong first-half register
// allocation, so fill_unmatched would link it and break boot-ELF byte-identity.
//
// Best-attempt C (compile -O4,p -sdatathreshold 4) — SECOND HALF (0x40-0x78) is
// byte-perfect; only the first packet's register allocation diverges:
//   extern int *D_00275670;
//   extern int D_00275674;
//   void func_001D2090(int a0, int a1) {
//       int *t0; char *p; int d674 = D_00275674;
//       t0 = D_00275670 + a0;
//       p=(char*)t0[4]; *(char*)(p+3)=0x30;
//       p=(char*)t0[4]; *(int*)(p+4)=d674;
//       p=(char*)t0[4]; *(short*)(p+0)=1;
//       p=(char*)t0[4]; t0[4]=(int)(p+0x10);
//       D_00275670[a0+0x14]=a1;
//       t0=D_00275670+a0; a0=0x50;          // idiom-16c: pin 0x50 to dead a0
//       p=(char*)t0[4]; *(char*)(p+3)=a0;
//       p=(char*)t0[4]; *(int*)(p+4)=a1;
//       p=(char*)t0[4]; *(short*)(p+0)=0;
//       p=(char*)t0[4]; t0[4]=(int)(p+0x10);
//   }
//
// WHAT WORKS: idiom-15 hoist of D_00275674 into an early local fixed the
//   instruction SEQUENCE (now identical); idiom-16c (a0=0x50 reuse of the dead
//   index param) made the entire second packet byte-exact.
// THE WALL: first-half `base` (D_00275670 reload) lands in $a3, but CW puts it
//   in $v1; this rotates the constant temps ($v1<->$a2<->$a3) for ~12 rows.
//   The leading temps (d674 in $t1, the 0x30/1 constants) raise first-half
//   register pressure so mwcc's global allocator assigns base $a3, while the
//   lower-pressure second half assigns base $v1 (matching). Falsified levers:
//   explicit base local declared first / reassigned each reload (idiom 3),
//   caching base for first half only, no-d674-hoist, plain literal vs param
//   reuse. Same family as the saved-register-allocation-ORDER wall datums in
//   docs/PROGRESS.md. Re-attempt with a new register-steering idiom.
