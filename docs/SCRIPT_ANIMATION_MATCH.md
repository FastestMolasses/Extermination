# Scripted-animation commit and light-buffer correction

## Interaction placement arguments

`func_001B6F00` now names the transformed interaction point and yaw offset,
and declares the matrix-vector helper's final argument as a pointer. That
argument was incorrectly named as an integer count. The120-byte function
still matches100% and remains ordinary compiled C. Its player-ground Y
retention and scratchpad writes are unchanged.

`func_00182F90` now describes and names its actual translation: subtract
actor+A0 from the target, add the delta to A0/B0/scratch3B40, then copy
Euler C0 to scratch3B50. The earlier comment incorrectly called the
temporary a transform and said C0 was another translated position. The
116-byte function still matches100%. The byte scratch declaration and
existing void-pointer helper prototypes are retained: two typed-float
spellings measured99.24138% and99.10345%, so they were not promoted as
exact replacements.

All six checks pass in `build/interaction_alignment/verify_all.log`,
including the complete boot executable and19/19 overlays. Fresh compiled
object provenance is recorded alongside it. These changes do not alter
the ordinary-C source or linked-slot counts.

## Clip initialization and explicit float arguments

`anim_clip_arbiter` now also declares and forwards both incoming float
arguments explicitly. Its previous C happened to preserve f12/f13 because
the function never touched them, but omitted those values from both its
signature and the initializer declaration. The corrected84-byte body remains
100% in objdiff and supplies ordinary compiled C to its96-byte linker slot.
All six checks pass again, including the entire boot loadable region and
19/19 overlays. Fresh source/object and prepared-text/relocation provenance
is recorded in `build/interaction_init/arbiter_link_provenance.json`; the
full gate is `build/interaction_init/arbiter_verify_all.log`. Classification
and linked-C counts are unchanged by this signature correction.

The subsequent continuation also converts `anim_clip_init` into212 bytes of
readable exact C and corrects `func_001749A0`'s omitted float arguments. The
latter now explicitly forwards the caller's blend and sets source frame0;
its old C omitted the real f13 initialization. A same-harness control scored
94.44444% with mwcc233, while the corrected72-byte function scores100%.
The initializer matches with mwcc233; the same source scores90.9434% with
the older compiler and96.22642% with mwcc24.

Both had explicit size-fallback entries. Removing those entries and running
all six checks confirms a byte-identical complete boot executable and19/19
overlays. Fresh link provenance verifies actual ordinary compiled-C objects,
fresh sources, matching prepared text and matching relocations for both.
The212/72-byte functions occupy224/80-byte padded linker slots. Evidence is
under `build/interaction_init/{verify_all.log,link_provenance.json}`.

The current source classification is1510 ordinary C,638 inline-assembly
wrappers,790 NEARMISS and15stubs. Of3010 boot slots,1224 now use ordinary
compiled C (314448 padded slot bytes),537 inline-assembly objects and1249
original-assembly fallbacks. All1761 copied objects match their prepared
text/relocations. Fresh objdiff is2051/2148 with matched_code98.16%.

This initialization is shared by startup and ordinary interaction animation:
it marks a transition in the clip ID, seeds the requested source-frame
channels, and resolves a zero-blend request immediately through the original
one-tick path. Nonzero blend retains the requested transition interval.
The native channel/matrix implementation still needs its separate original
runtime comparisons; these linked-byte checks do not certify the native port.

## Earlier commit and buffer correction

`func_00183090` now uses readable C and supplies its actual compiled208 bytes
to the boot ELF. Canonical objdiff is100%, and the fresh link-provenance audit
confirms ordinary compiled C with matching prepared text and relocations.
All six checks pass in `build/interaction_animation/verify_all.log`, including
the complete boot loadable region and all19 overlays.

The original function distinguishes a changed ordinary clip request from an
unchanged request. A changed request initializes its transition, clears the
animation flags and returns0, suppressing another advance during the same
player callback. An unchanged request returns1, permitting advancement.
Special bone modes1/3 become2/4 and also permit advancement.

An initially equivalent C body scored97.88461% because the compiler removed
a redundant byte mask and loaded the mode directly into a0. Declaring the
mode as int and writing `mode &= 255` inside the nonzero branch reproduces
the original lbu/zero-test/andi sequence and register choices. Placing that
conversion before the test, signed/unsigned declarations alone, the three
available compiler versions and a shared-return spelling did not match.
Seventeen bounded variants are recorded under `build/interaction_animation/`.

This converts one inline-assembly unit to ordinary C; the overall object
match count stays2050/2148 because the old wrapper already matched. Source
classification becomes1509 ordinary C,639 inline assembly,790 NEARMISS and
15stubs. The3010-slot link audit selects1222 ordinary compiled-C objects
covering314,144 bytes,537 inline-assembly objects and1251 assembly fallbacks.
This is link provenance, not a claim that every recovered function is correct.

The same gate includes a semantic repair to NEARMISS `func_001D7C30`:
its local scratch matrix is16 floats, previously4. The original identity and
rotation helper writes require64 bytes. A host ASan/UBSan contract check
reproduces the old stack-buffer overflow and passes with the corrected extent;
reports are under `build/point_light_reference/`. The source remains a measured
near-match and uses original assembly in the linked game. Native light-pool
and SDK-instruction comparisons are documented separately in the port's
`docs/AREA11_POINT_LIGHT.md`.
