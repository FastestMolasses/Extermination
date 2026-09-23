# Original status ammunition display

`src/func_00209860.c` remains a readable near match with the original
assembly linked. The actual caller `209DF0` passes `(ui, 16, 190)`;
the previous two-argument definition consumed the wrong coordinate
registers. The corrected signature retains the initial unused UI argument.
The normal secondary number uses a three-place field. The reserve number
and secondary fuel percentage use four places.

The native repository's `tools/test_status_draw_reference.py` executes
the original `209860` and numeric formatter and compares every emitted
blend, rectangle, sprite and text call with the native core. Its 528 ammo
cases also execute the canonical C through
`tools/status_ammo_source_probe.py`. The host shim widens pointer-through-int
arguments to `intptr_t`; all tested scalar intermediate values fit signed32.
String copy/append and final drawing are explicit worker boundaries.
Tests cover the primary override, all supported secondary selectors,
negative counts, over-width fields and percentage alignment.

Invalid secondary selectors with primary other than2 have no defined
asset choice: the original retains incoming `s0` as TEX0 while using the
reserve count. Six different selector/register combinations demonstrate
that inherited value. The readable C retains the corresponding unresolved
local; it does not claim a defined portable behavior for that state. The
native display core faults before drawing unknown selectors. Original
assembly remains the authority for this caller-dependent path.

With mwcc233 `-O4,p -sdatathreshold 0`, the corrected source measures
95.430595% similarity: 1,436 candidate bytes against 1,412 original bytes,
up from the previous 92.08% report. This is not a new compiled-C match.
Ignored receipts are `build/status_draw/ammo_result.json` and
`ammo_diff.json`; full binary/overlay verification is recorded in
`build/status_draw/verify_all.log`.
