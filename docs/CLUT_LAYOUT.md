# Resident palette decoding correction

The native snow smoke exposed a shared texture-export error: the original
16x16 flake became a hollow ring. `gs_vram.csm1_unswizzle_clut` exchanged
CSM1 index groups directly on physical VRAM bytes, omitting the PSMCT32
block/column address decode.

A resident 256-entry palette requires two operations: read its 16x16 image
through PSMCT32 addressing, then map that image's CSM1 arrangement to index
order. The previous 16-entry PSMT4 reader already handled its 8x2 physical
layout correctly. The hardware arrangement is specified in Sony's
[GS User's Manual 6.0, sections 2.7.3 and 8](https://www.scribd.com/document/784545197/GS-Users-Manual).
No emulator implementation source was used.

The local original opening state binds snow to TBP8909, CBP8368, PSMT8.
The corrected extraction maps its center index15 to RGB204; the old helper
mapped it to RGB12. The extracted texture now has the original soft solid
shape. Images and palette data remain in ignored build/assets directories.

`python3 tools/test_clut_layout.py` checks all256 synthetic index addresses,
nonzero base-pointer addressing and alpha expansion, rejects incomplete
palettes, and cross-checks the first16 entries against the independently
implemented PSMT4 reader. All three tests pass.

The six first-level render files contain290 texture bindings:289 use PSMT4
and one movable binding uses PSMT8. Regenerating the latter preserves all
151,144 header/geometry/animation bytes and changes6,351 texture bytes.
Opening actors/faces, pickup bodies/lights, panel/elevator props and snowfall
were also regenerated through the corrected helper. The change does not
validate texture residency, every material's blend state, or scene lighting.
Other existing asset directories need regeneration from their original
inputs before they can benefit; an old EMDL does not update itself.
