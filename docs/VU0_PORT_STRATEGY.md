# Hand-written assembly: why it exists, and how the port handles it

~66 of the remaining stubs are VU0 macro-mode, MMI (128-bit SIMD), or COP0 privileged code.
This document explains why they cannot become byte-matching C, and what the native port does
with them instead. It is the ground-truth contract between the decomp and the port.

## Why these cannot be C

A matching decompilation requires that the compiler, fed our C, emits the *exact* original
instruction sequence. That is only possible for instructions the compiler will actually
generate. Three classes fail that test:

**VU0 macro mode (COP2).** The Emotion Engine has a vector coprocessor addressable directly
from EE code (`vadd`, `vmul`, `vsqrt`, `vrsqrt`, `vopmula`, ...) operating on 128-bit
`$vf0`–`$vf31` registers with per-component write masks (`.xyzw`, `.xyz`, `.x`). CodeWarrior
2.3.1.01 has no C construct that emits these — no intrinsic, no autovectorizer. They appear
only because a human wrote them. In our splat output they show up as raw `.word` encodings
with the mnemonic in a comment, which is itself a tell.

**MMI (multimedia instructions).** 128-bit integer SIMD (`paddw`, `pextlw`, `pcpyld`,
`pmaxh`, ...). *Careful here:* the presence of `lq`/`sq`/`qmfc2` does **not** prove
hand-written code — mwcc emits those for ordinary 128-bit stack spills and struct copies in
plain C. Only *arithmetic* MMI indicates real vector work. Getting this wrong overstates the
hand-written set; see docs/STUB_INVENTORY.md, where the distinction moved 5 functions back
into the decompilable pool.

**COP0 privileged ops.** `mfc0`/`mtc0`, `ei`/`di`, cache and TLB control. No C form exists
for the same reason: the compiler will never emit them.

So the reasoning is right — these need hand-written assembly to reach byte-identity, and the
byte-identity requirement is what makes the decomp a true decompilation. The subtlety is that
"cannot be byte-matching C" is not the same as "cannot be understood." For the port we need
the second thing.

## The two artifacts are different products

The decomp and the port want opposite things from these functions, and conflating them is the
mistake to avoid:

- **The decomp** needs the exact original bytes. It gets them by linking the function from its
  address-pinned `.s`. Nothing to do; this already works and the boot ELF is byte-identical.
- **The port** needs to know *what the function computes*, not how the EE encoded it. A
  faithful port reimplements the operation, it does not emulate the instruction.

That means the deliverable for each hand-written function is a **semantic spec**, not a C
translation. A spec that says "normalizes a 3-vector, writing 1.0 to w" is portable to NEON,
SSE, or scalar C. A transliteration of `vrsqrt` into a chain of intrinsics is not, and it
inherits PS2 quirks nobody asked for.

## The floating-point trap

PS2 floats are **not IEEE-754**, and this is the single biggest fidelity risk in the port:

- No denormals — they flush to zero.
- No infinities or NaN — operations **saturate** to the largest representable magnitude.
- Round-toward-zero only, not round-to-nearest-even.
- `vrsqrt`/`vsqrt` are low-precision approximations with their own error curve.

Compiling the "same" arithmetic with a modern IEEE-754 compiler therefore produces
*different numbers*, not just differently-encoded ones. Where results feed geometry or
animation, small divergences are invisible; where they feed comparisons, physics thresholds,
or RNG, they change behaviour.

The strategy, in order of preference per function:

1. **Reimplement in portable C/SIMD** where the operation is numerically forgiving
   (transforms, interpolation, matrix multiply). Fastest and cleanest.
2. **Reimplement with an explicit PS2-float shim** where results feed control flow — a small
   library implementing saturation and round-toward-zero, used only in the functions that
   need it. Pay the cost where it buys fidelity, not everywhere.
3. **Keep an interpreter for the genuinely exotic** (a handful of VU0 sequences using the
   accumulator and `vopmula`/`vopmsub` cross-product idiom in ways not worth restating).

Decide per function, and record the decision *in the spec* — that is what makes the choice
reviewable later.

## Spec format (one file per function)

```
# <name>  (vram 0x........, N instructions, class: VU0 | MMI | COP0)

## Signature
Inputs:  a0 = <type/meaning>, $vf4 = <what it holds on entry>, ...
Outputs: v0 = ..., memory written at ..., $vf8 = ...
Clobbers: ...

## What it computes
<Plain-language statement of the mathematical operation. This is the load-bearing
part — write it so someone could reimplement from this alone.>

## Numerical notes
<Anything where PS2 float behaviour is observable: saturation, RTZ, vrsqrt precision,
division by zero. State whether results feed control flow.>

## Port decision
<reimplement-portable | reimplement-with-ps2-float-shim | interpret> — and why.

## Verification
<How to prove the port matches: reference vectors captured from PCSX2, a property
that must hold, or a differential test against the interpreter.>
```

## Verification is the part that is easy to skip

A spec nobody tested is a guess. For each ported function, capture input/output vectors from
the original running under PCSX2 and assert the reimplementation reproduces them. For the
numerically-forgiving class an epsilon comparison is fine; for anything feeding control flow
it must be exact under the PS2-float shim. Build this harness before porting in bulk — it is
what turns "we think this is right" into evidence, and it is the same discipline that made
the matching side trustworthy.

## Status

Specs are not yet written. The inventory (docs/STUB_INVENTORY.md) identifies the ~66
functions in scope: 36 VU0, ~30 genuine-SIMD MMI (minus those reclassified as ordinary C),
3 COP0. This is the largest remaining work item for the port and is independent of the
matching effort — it can proceed in parallel.
