# Extermination — Code Subsystem Map & Decomp Frontier

A navigation map of the game's code, built from the `jal` call graph over the
splat disassembly. Identifies the shared core helpers, the subsystem clusters,
the per-frame orchestrator tree, and — most importantly — **the real
decompilation frontier**.

Project annotation derived from our own call-graph analysis; no disc data.

---

## The decompilation frontier (read this first)

The matching pipeline makes the boot ELF byte-identical by **assembling the
splat disassembly** for any function that has no compiled `src/*.c`. That keeps
the rebuild byte-perfect, but those functions are **not decompiled** — there is
no C for them at all, readable or otherwise.

Game-code functions (vram ≥ 0x00130000): **2063 total.**

| Tier | Count | Meaning |
|------|------:|---------|
| Has `src/*.c` | 765 (37%) | decompiled to C (readable) **or** matched as opaque `.word`/asm-void |
| **Not in src** | **1298 (63%)** | **undecompiled** — only the original disassembly, re-assembled |

So the headline `matched_code 96.18%` is measured over the **765-with-src**
units only. True readable-C coverage of the *whole* game is far lower. **The
1298 undecompiled functions are the genuine remaining decomp work** — and the
breadth `.word`→readable campaigns never touched them (they only rewrote
*existing* `.word` src files).

Size of the undecompiled frontier: **127 small (≤128 B), 576 medium
(128–512 B), 595 large (>512 B).**

**Highest-leverage decomp order:**
1. The **most-called undecompiled helpers** (below) — each clarifies dozens–
   hundreds of call sites and the readable C that already calls them.
2. The **127 small undecompiled functions** — fast, high-yield coverage.
3. Medium functions, by subsystem, using the cluster map.

---

## Core helper library (most-called game-code functions)

These are the shared primitives. `status`: R=readable C, W=`.word`/asm match,
**U=undecompiled (no src — prime target)**.

| Callers | Function | status | identified role |
|--------:|----------|:------:|-----------------|
| 211 | func_001FBD50 | W | **play_sound**(obj, id, 0, range) — most-called fn in the game |
| 157 | func_001B1470 | W | math/vector helper (0x1B1 cluster) |
| 108 | func_001749A0 | R | **anim clip commit** |
| 106 | func_001AFC10 | **U** | **object-process** (the dispatch tail) — high value |
| 87 | func_001B12B0 | W | vector/math helper |
| 78 | func_001B1240 | R | **heading angle from entity→target vector** |
| 67 | func_001B17A0 | **U** | vector/math helper |
| 62 | func_001CFBE0 | **U** | (0x1CF cluster) |
| 60 | func_00175900 | **U** | entity/anim helper |
| 51 | func_001EFD90 | W | (0x1EF cluster) |
| 48 | func_001CFB50 | **U** | (0x1CF cluster) |
| 48 | func_00178B90 | W | entity/anim helper |
| 46 | func_001C6380 | W | anim subsystem (0x1C6) |
| 42 | func_001B61C0 | **U** | math helper |
| 41 | func_0019A570 | **U** | (0x19A cluster) |
| 40 | func_001FB9F0 | **U** | **sound cue** (0x1FB sound cluster) |
| 39 | func_00179880 | R | **clamp** |

The undecompiled (**U**) entries here are the single highest-leverage targets:
e.g. `func_001AFC10` (object-process, 106 callers) and `func_001FB9F0` (sound
cue, 40 callers).

---

## Subsystem clusters (by address range + role)

Functions cluster tightly by vram, each range a subsystem. Named/identified
anchors in **bold**.

| vram range | subsystem | anchors |
|------------|-----------|---------|
| 0x1FBxxx | **audio / sound** | **func_001FBD50 play_sound**, **func_001FB9F0 cue** |
| 0x1B1xxx–0x1B6xxx | **math / vector** | func_001B1470, func_001B12B0, **func_001B1240 heading** |
| 0x1C6xxx–0x1CBxxx | **animation** | **anim_eval_skeleton, anim_sample_bones, quat_nlerp, quat_to_mat3, build_trs_matrix** (named) |
| 0x174xxx–0x17Bxxx | **entity / motion / anim glue** | **func_0017D8D0/DE20 motion update**, clip selectors, **func_00179880 clamp** |
| 0x15Bxxx | **per-frame top (anim pipeline)** | **anim_frame_top_a/b** (func_001ACA20/001AE040, named), func_0015B130/BCF0 |
| 0x100xxx–0x102xxx | **DMA / VU1 / GS plumbing** | **dma_kick, dmac_channel_base, gs_vram_readback, vu1_cold_start** (named) |
| 0x1D1xxx–0x1E9xxx | **render / VIF packet build** | **vif_append_ref_tag, vif_build_unpack_const** (named) |
| 0x228xxx–0x229xxx | **memcard / save** | **sub_BASCUS_97112_DS00_* ** (named, EX_DATA save files) |
| 0x1DBxxx–0x1DCxxx | **weapon / HUD labels** | **sub_NIGHT_VISION_SYSTEM, sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER** (named) |

---

## Per-frame orchestrator tree (functions calling the most distinct targets)

| Callees | Function | role |
|--------:|----------|------|
| 44 | **func_001AAE40 (gs_readback_queue_run)** | per-frame GS-VRAM readback queue |
| 43 | func_0015B130 | per-frame anim/entity dispatch |
| 41 | **func_001662D0** | per-frame entity setup (calls 41 helpers) |
| 38 | **func_001AE040 (anim_frame_top_b)** | per-frame anim top |
| 37 | func_001647D0 | entity/level update |
| 31 | func_00168050 | entity/level update |
| 30 | func_0015B770 | per-frame dispatch |

The per-frame update spine is the `0x15Bxxx` + `0x166xxx` + `0x168xxx`
orchestrators; the native port's main loop will hook here (drive the entity
update, then translate the geometry/draw the orchestrators produce).

---

### Method

`jal` edges parsed from `build/asm/matchings/main/code/*.s` (2827 functions,
2255 with callers). "not in src" = no `src/<name>.c` resolves to the function's
vram (checked against renamed/named files via `config/symbol_addrs.txt`).
Regenerate by re-running the call-graph scan in the session notes.
