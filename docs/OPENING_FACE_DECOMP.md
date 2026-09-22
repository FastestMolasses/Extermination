# Opening face decompilation audit — 2026-09-22

Two readable decompilations had semantic errors despite headers describing
their differences as unavoidable compiler artifacts. Their behavior is now
corrected and their residual classification measured again. Both remain
NEARMISS: the rebuilt game still links their original assembly fallback.

| Function | Corrected object match | Compiler / flags | C text / original |
|---|---:|---|---:|
| `func_001D3E40` | 81.26471% | mwcc 991202; `-O4,p -sdatathreshold 4` | 276 / 272 bytes |
| `func_001D0720` | 97.27811% | mwcc 2.3.3; `-O4,p -sdatathreshold 0` | 1376 / 1352 bytes |

The previous source headers reported 71.07% and 97.26% respectively (the face
registry row was stale at 90.72% and incorrectly described a particle effect). These
object percentages do not establish semantic equivalence or compiled-C
coverage in the linked game. No new executable compiled-C match is claimed.

## Corrected behavior

`func_001D3E40` selects the face morph program by passing its DMA **channel
index** to `vif_append_ref_tag`. The previous C passed `model + 0x40`, the mesh
packet address. That helper interprets its first argument as a channel index
into the renderer context, making the substitution invalid. The subsequent
tags submit scene constants, an optional two-qword setup block, and the mesh
packets. The corrected C names the channel, model, packet cursor and offsets
according to those roles.

`func_001D0720` handles an unknown second-state value through the same talk
gate as state4. With talking enabled, this path resets that state to0. The
previous C handled state4 but omitted the switch default, leaving an unknown
state unchanged. An explicit shared label now preserves the original branch.
The asymmetric mouth choice0..4 versus targets1..5 remains unchanged.

## Independent checks

Bounded interpreters execute the original user-owned instructions and the
relocated compiled candidates. Their local scripts and reports are under
`build/opening_face/match/`; they contain no embedded original instructions.

- DMA: 384 cases compare the helper arguments, packet bytes and final cursor,
  covering four channels, four scene indices, six quadword counts and four
  optional-setup return values. All pass.
- Face: 7,680 complete state transitions compare all face-state bytes and
  5,213 RNG calls across state/timer/speed/talk boundaries, including negative
  and otherwise unknown expression states. All pass.

The face interpreter uses host IEEE float32 instruction arithmetic. It proves
the branch and operation sequence under that arithmetic; it is not an EE
floating-point rounding emulator. The native opening face audit additionally
checks 11,552 states and 26,880 original VU morph blends; see
`OPENING_ACTORS.md` for geometry and remaining runtime fidelity limits.

The compiler sweep tried the three locally installed CodeWarrior versions,
small-data thresholds0/4/8, and equivalent local-lifetime/control-flow forms.
The retained residuals include GPR/FPR allocation, reload and branch-delay
scheduling, and additional instructions. They are working descriptions,
not a claim that further source changes cannot improve the match.

## Executable gate

Run the complete local suite with `.venv/bin/python tools/verify_all.py`.
The audit's full output is `build/opening_face/verify_all.log`. All six stages
passed before the isolated source commit: the boot ELF's `0x175b00` loadable
bytes are identical, all19 overlays are identical, the existing object tracker
passes its floor (98.14%; 2050/2148 units), and glTF, decoder self-test and GS
offset checks pass. The tracker includes assembly-backed units and is not a
count of verified readable C. These two corrected functions remain excluded
as NEARMISS assembly fallbacks.
