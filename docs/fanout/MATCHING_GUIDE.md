

Third batch, same wave (14 byte-matched, all compiled_object_ordinary_c):

- **ee-gcc SDK RPC/SIF family** (func_00113280, func_00111C28, func_001140C8,
  func_00111AE0, func_00106830, func_00106B88): the inline `asm("nop")` in
  busy-wait delay loops was never in the source (the assembler pads short
  loops; the extra statements changed register allocation); the latch reset
  belongs inside the retry loop before `break`; identical failure tails are
  separate blocks (gcc merges them), not a goto; RPC clients and request
  buffers are struct globals used directly; the debug-print callee returns
  int; func_00106490 takes no argument (func_00106830: 60.94 -> 100).
  func_00106B88's saved-register order came from where `p = &D_0024147C;`
  sits between two stores (a short ee-gcc permuter run found it).
- **Wrong calls and bodies**: func_00135870 (anim_clip_init arguments,
  four-argument func_001FBD50), func_0018B3E0 (four callee prototypes and a
  3.34f constant), func_00151940 (the cooldown branch's state choice),
  func_001E9E60 (twin of func_001E9280, plus a `switch` with `default:`
  first, idiom-33).
- **Spelling**: func_001C1A80 (convert inside the argument expression),
  func_00131F90 (one float local for both bucket calls; `<= 0xA`),
  func_001FFCD0 (cursor added last as an unsigned int; `n1 += n2`; its
  unprototyped `()` externs must stay unprototyped), func_0020B210 (one shared
  counter, row byte read first), func_00201C50 (early returns, not a preset
  result).
- Callee definitions that disagree with byte-matched callers (not changed,
  noted for whoever reconciles them): func_0021BE40 is defined with one
  parameter but every caller passes (player, self); func_0020CDA0 is defined
  `(void)` but callers set $a0 / $a1 (func_0020B210 keeps a value in $a0,
  func_00201C50 passes g + 0xD, g).
- fill_unmatched.py: func_001E9E60 and func_001C1A80 dropped from
  GPREL_FORCE_ASM (same proof as func_001E9280).
- Automated prototype scan used to order the queue: every NEARMISS extern
  whose parameter count differs from the callee's byte-matched definition
  (unprototyped `()` declarations excluded). About half the hits were real.
