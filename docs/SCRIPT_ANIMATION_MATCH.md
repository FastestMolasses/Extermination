# Scripted-animation commit and light-buffer correction

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
