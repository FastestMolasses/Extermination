# Interaction camera seed (0018CBD0)

The previous readable NEARMISS applied a target-height falloff even when
horizontal distance was not compressed, and inverted the falloff clamp.
Its claim of complete semantic recovery was incorrect.

The corrected function initializes falloff to zero. Only when the horizontal
distance minus `abs(distance)` is below the preset threshold does it compute
`threshold + (threshold - delta)`, capped above at -7. The normal preset
uses threshold -20, target-height addition6 and eye-height addition2;
the other preset uses -10,2,6. The final common base is11.

For the original AREA11 panel capture, scratchpad rotation is zero, current
distance and preset distance are both -46.8, and player+A0 is
(239.69999695,229.89044189,223.80000305). The corrected original instruction
path produces target Y246.89044189 and eye Y248.89044189, exactly the
captured camera vectors. Player+B0 is the animated hip and must not be
substituted for the actor position.

`extermination-port/tools/test_camera_retarget_reference.py` executes the
original instructions at0018CBD0..0018CE54 from the user's pinned ELF.
The isolated native scalar seed matches all948 bounded cases in float
bits, including both presets, signed distances and compression branches.
It also matches the captured panel camera. This oracle supplies the
transformed offset at the original matrix/vector helper boundary and
uses host sqrtf in both paths. It does not establish SDK rotation or
square-root identity, physical EE rounding corner cases, or collision
solver equivalence.

The corrected readable C measures94.04% with mwcc233, up from91.78%.
Remaining object differences are unclassified. It remains marked NEARMISS
and assembly-backed; this is not a matched-function promotion. The full
six-stage gate passes: loadable boot bytes identical,19/19 overlays,
matched-code98.14%, glTF/animation checks and GS-offset checks.

Panel opcodeD/sub3 next calls0018D7B0 in styles5 then1, then stores120
at camera+A0. Style5 runs D330 and D910 without publishing actual vectors;
style1 runs D330 and DD20, then copies desired target/eye to the actual
vectors. The existing native follow solver includes chase behavior and
cannot stand in for this sequence. The legacy door helper's +11 height
claim was removed; its behavior remains an explicitly unverified older
approximation pending its own script adapter.
