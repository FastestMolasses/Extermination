# ITEM glow draw-context correction

Original001D66A0 emits an untextured additive Gouraud fan as alternating
center/outer vertices in a triangle strip. The prior readable source called
it a particle initializer and treated D275670 as an array. The executable
loads a context pointer and selects the cursor at context+0x10+4*slot.
The corrected declaration and indexing remove the extra slot scaling.

The corrected ordinary C measures68.564415% with mwcc233, O4,p and
sdatathreshold0, up from67.06%. It remains NEARMISS and assembly-backed;
the generated source is not selected for the linked executable. Scalar
expressions do not yet reproduce the original MAC instruction sequence.

All six checks in tools/verify_all.py pass after this correction; local
build/item_root/verify_all.log records the gate. The linked boot load segment
remains byte-identical, all19 overlays pass, and the reported matching
coverage remains98.16%/2051 of2148 tracked functions. The native companion's
test_item_trail_reference.py separately validates packet geometry with explicit
SDK transcendental boundaries; that is not a C byte-match or raster certificate.
