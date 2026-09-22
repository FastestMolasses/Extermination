# Quaternion blend matching follow-up

`quat_nlerp` at 0x001CA0A0 remains readable C marked NEARMISS. Its name is
historical: the original upper-clamps the blend factor, selects the quaternion
hemisphere by the dot-product sign, and blends without normalizing. The
cinematic bone transforms depend on retaining that non-unit result.

The canonical mwcc 2.3.3 build is 276 bytes and scores 99.05797%. Comparing the
entire function's raw bytes finds exactly two differing instruction words,
at offsets 0x14 and 0x18. The generated conditional branch targets four bytes
later and materializes the second 1.0f constant in its delay slot. The original
leaves that slot empty. The other 67 instruction words are identical.

A bounded follow-up tried equivalent clamp forms using loops/breaks, explicit
joins, Boolean dispatch, local constants, inline helpers, compound assignments,
and constant representation changes. None improved the canonical result.
The available mwcc 2.4 compiler also gives the same two-word difference with
optimization levels 3 and 4, both speed and size settings. Debug information
does not change it. Disabling instruction scheduling restores the first branch
but changes the body substantially (62.855072%, 284 bytes); disabling peephole
optimization gives 53.391304%, 316 bytes. Scoped inline-helper optimization
pragmas did not isolate the desired change. Unsupported command-line options
were diagnosed as unsupported and are not evidence of optimizer behavior.

The best remaining explanation is the available compiler's delay-slot
scheduling policy. This is evidence for a bounded compiler limitation, not a
proof that no readable source can ever match. The simple canonical C is retained;
no inline assembly, object patching, fallback suppression or matching-statistic
promotion was introduced. The original assembly remains selected by the linker.
A future actual compiled-C promotion requires raw whole-function identity and
the complete six-stage verification gate.

Local ignored search evidence: `build/quat_followup/results.json`, `summary.log`,
the generated candidate C files and compiler logs. `build/opening_quat_match`
contains the prior baseline. These directories hold local original-reference
objects and must remain untracked.
