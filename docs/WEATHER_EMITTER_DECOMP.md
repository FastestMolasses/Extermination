# Weather tile emitter semantic correction

`func_001E67C0` emits a camera-relative weather grid. Its previous description
incorrectly called it an explosion/impact and audio spawner. `0021B9A0`
configures fog; `001CFFE0` submits the original VU particle program.

The original `001CFAE0` call passes these four floating arguments:

| Register | Original value | Previous incorrect C |
| --- | --- | --- |
| f12 | Current row phase | 1 |
| f13 | Local seed fraction + 0.0001 | Row phase |
| f14 | 1 | Seed fraction + 0.0001 |
| f15 | 0.000001 | 1 |

The helper stores these into the VU parameter qword as phase, color multiplier,
fade-in interval, and seed fraction. Incorrect seed mantissas change particle
positions; the phase and fade values are also necessary for the original
motion. Locals now describe rows, depth/vertical steps, phases, and draw state.

## Match measurement

The corrected function remains **NEARMISS**, with the original assembly linked.
No claim is made that readable C now reproduces the original instruction bytes.
Measured using mwcc 2.3.3, `-O4,p -sdatathreshold 0`:

| Source | objdiff similarity | C text bytes | Original bytes |
| --- | --- | --- | --- |
| Prior incorrect arguments | 71.55123% | 1776 | 1952 |
| Corrected arguments | 70.915985% | 1784 | 1952 |

The lower similarity is recorded honestly; retaining incorrect semantics to
preserve a larger percentage would not improve fidelity. Additional mwcc
991202 and small-data variants were measured; none exceeded the selected
correct candidate. Earlier unsupported claims of a compiler-impossible
register-allocation artifact have been removed.

## Validation

A bounded instruction interpreter compares the relocated, compiled corrected
C against the original function in the owner's ELF. Across 24 combinations of
AREA11, AREA15, AREA21, reversed phase direction, and the AREA21 camera cases,
the compiled candidate produces identical controller state and 2,376 complete
tile submissions. This comparison shares intercepted SDK/matrix helper
semantics and finite arithmetic assumptions; it is not a PS2 floating-point
emulator or proof of identical pixels.

Separate native validation compares 5,184 tile records with original emitter
instruction flow, and 216 captured DMA parameter/color qwords plus phase
transitions. It also checks the original SDK waveform separately and reports
remaining matrix errors. See the sibling native repository's
`docs/SNOW_PARTICLES.md` for runtime evidence and rounding/camera limitations.

Local measurement and verification logs are ignored under
`build/weather_reference/match`. Original code/data and generated object files
are not included in this source change. The full six-stage original-ELF gate
passed: all 0x175b00 boot loadable bytes and all 19 overlays remained identical;
the match-report guard, glTF round trip, animation self-test, and GS-offset
check also passed. The reported 98.14% match tracker is not a measurement of
readable-C coverage.
