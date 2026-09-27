# Extermination — Runtime Structures (reverse-engineered)

Game runtime structures recovered from the **readable-C decompilation** in
`src/` (byte-matched and NEARMISS function bodies). Offsets are from
the structure base pointer; widths are from the access instruction
(`lb/sb`=u8, `lh/sh`=s16, `lw/sw`=int/ptr, `lwc1/swc1`=float, `lq/sq`=128-bit).
Confidence is noted per field. Cross-reference: the **bone / skeleton** struct
is documented in `docs/FINDINGS.md` (anim evaluator section).

This file is project annotation derived from our own decompiled C — no
disc-derived data.

---

## Entity / object struct

The dominant game-code structure: passed as the first argument (`obj` / `e`,
EE register `$a0`) to most game-logic functions. Sub-objects are the **same
type**, linked through `+0x20`. The struct is large (≥ 0x320 bytes); only the
fields touched by decompiled functions are known so far.

**Player vs enemy position.** Enemy actors use `+0xB0/+0xB4/+0xB8` as their
position (e.g. func_001551B0). The player actor (`D_008102B0`) uses `+0xA0` as
its actor position and `+0xB0` as the animated hip: range checks pass
player+0xA0 against entity+0xB0 (FINDINGS "Senses & the player light"), and
the camera seed func_0018CBD0 reads player+0xA0 (see its src header).

| Offset | Width | Field (meaning) | Confidence | Seen in |
|--------|-------|-----------------|------------|---------|
| +0x04 | u8 / int | **state** — main state-machine selector (1, 4, …). The switch key in per-frame entity dispatchers. | high | func_001838B0, dispatch family |
| +0x05 | u8 | sub-state | high | func_001838B0 |
| +0x06 | u8 | flag | med | func_001838B0 |
| +0x20 | ptr | **pending sub-object** (same struct type). On detach its `+0x04` is set to 3 and this slot is cleared. | high | func_00131E80 + teardown family |
| +0x28 | s16 | **per-state countdown timer** — generic; e.g. the motion updaters count it down and snap position to the target at 0, and the shoulder-light state machine uses it as its 300-frame delay. | high | func_0017D8D0, func_0017DE20, func_00161020 |
| +0x9A | u8 | subsystem **dispatch id** (passed to func_001B1190). | high | teardown/dispatch family (11 fns) |
| +0xB0 | float | **position.x** | high | motion updaters |
| +0xB4 | float | **position.y** | high | motion updaters |
| +0xB8 | float | **position.z** | high | motion updaters |
| +0x1F1 | u8 | substate (set to 2 on reset) | med | func_0016F5D0 |
| +0x1F2 | s16 | **previous clip id** (snapshot of current clip on a transition) | high | func_001838B0 |
| +0x20C | s16 | **current animation clip id** | high | func_001838B0, clip selectors |
| +0x258 | float | **snap target.y** (motion endpoint) | high | func_0017D8D0/DE20 |
| +0x26C | float | lateral push (added to pos.y, bleeds off by +0x270/frame) | high | func_0017DE20 |
| +0x270 | float | push decay rate | high | func_0017DE20 |
| +0x2E0 | float | **velocity.x** | high | motion updaters |
| +0x2E4 | float | **velocity.y** | high | motion updaters |
| +0x2E8 | float | **velocity.z** | high | motion updaters |
| +0x2F1 | u8 | substate (clip selector flag) | med | clip selectors |
| +0x2F2 | u8 | substate (set to 0 on reset) | med | func_0016F5D0 |
| +0x2F4 | float | **snap target.x** | high | func_0017D8D0/DE20 |
| +0x2F8 | float | **snap target.z** | high | func_0017D8D0/DE20 |
| +0x318 | u8 | substate (set to 2 on reset) | med | func_0016F5D0 |

Notes:
- **Transform block.** position `{+0xB0, +0xB4, +0xB8}`, velocity
  `{+0x2E0, +0x2E4, +0x2E8}`, snap target `{+0x2F4, +0x258, +0x2F8}`
  (x, y, z). The standard motion update is: decrement `+0x28`; on the final
  tick set position := target and return 1; otherwise integrate velocity into
  position (plus the decaying lateral push on y) and return 0.
- **State machine.** `+0x04` is the primary state; `+0x05/+0x06` and the
  scattered substate bytes (`+0x1F1/+0x2F2/+0x318`) gate per-frame behaviour.
  Per-frame entity dispatchers switch on `+0x04` then run a motion+anim tail.
- **Animation.** `+0x20C` current clip, `+0x1F2` previous clip; clip changes
  go through the anim-clip helpers (below) with a blend weight.

## Common helper functions (revealed by the readable C)

| Function | Signature (inferred) | Role |
|----------|----------------------|------|
| func_001AFC10 | (obj) | process/update an object — the dispatch tail |
| func_001B1190 | (dispatch_id) | notify a subsystem by id (`obj[0x9A]`) |
| func_001662D0 | (e) | per-frame entity setup |
| func_00174A50 | (e, float blend) | resolve/commit animation clip with a blend |
| func_001749A0 | (e, …) | animation commit (clip selectors call it) |
| func_00179B90 | (obj) -> int | get the entity's sound-base id |
| func_001FBD50 | (obj, sound_id, flat2d, float radius) | play a positional sound — radius = attenuation range (300.0 typical); full vol/pan math decoded (FINDINGS "Engine 3-D VOLUME/PAN") |
| func_001749F0 | (e, …) | anim clip-change arbiter (see FINDINGS) |

## Related globals

| Symbol | Meaning |
|--------|---------|
| D_008106C7 | **gun-light draw enable**: mirrored from D_00810D3C by func_0017A970 (set on the rising edge, cleared on the falling edge), read by func_00188ED0 to draw the light cone; also cleared by func_0016F5D0 / func_0018A6B0 (FINDINGS s87 gun-light correction) |

## Bone / skeleton struct — see `docs/FINDINGS.md`

The per-bone animation struct (distinct from the entity struct above) is
documented in FINDINGS.md "Per-bone animation evaluator": `+0x30` quat A,
`+0x40` quat B, `+0x50` blend t, `+0x60` translation, `+0x64` s16 parent index
(-1 = root), `+0x70` euler, `+0x88..0x8C` s16 Q4.12 fine scale, `+0x90` output
4×4 matrix. The actor carries `+0x0C` bone_count, `+0xD0` root matrix, and
`+0x110[]` the per-bone struct pointer array.

---

### How this was derived

Each field comes from a byte-identical decompiled function that accesses it —
e.g. `*(float *)(obj + 0xB0) += *(float *)(obj + 0x2E0)` in the motion
updaters establishes position.x and velocity.x. As more functions become
readable C, extend the table.
