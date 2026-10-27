

Second batch, same wave (20 byte-matched, all compiled_object_ordinary_c):

- **idiom-32 again**: anim_matrix_dispatch (D_70003A20 as `extern float
  D_70003A20[4]` under `-sdatathreshold 4`; scalar goes gp-relative),
  func_0012DE90 (one extern per scratchpad address: element offsets of one
  symbol score below 100 with identical code), func_001809B0 (83.35 -> 100:
  real 5-argument func_00180600 prototype, return values, block contents,
  idiom-24, then the scratchpad words as externs), func_001F1DD0.
- **idiom-24 (constant written as an int converted to float)** fixes argument
  constant order: func_00162DB0 / func_0016C6A0 (`int deg = 4; float step =
  (3.1415927f * (float)deg) / 180.0f;` sets $f14 before $f12 = 0),
  func_001662D0, func_00230230, func_0014E050 (deg = 2), func_0014A0D0.
- **Same-file static callee (`// TU_STATIC:`) for one value in another
  argument register**: func_00169730 (func_001696A0, `-sdatathreshold 0x20`)
  and func_0016AE40 (func_0016ADE0, `-sdatathreshold 4`); the callee's own
  threshold is required.
- **`break` instead of `return` in switch cases**: func_0014E050 and
  func_0014A0D0 (the recorded "case-0 dup-add wall").
- **Wrong extern or call**: func_001066F8 (ee-gcc; func_00106490 takes no
  argument), func_001E9280 (func_001CB6B0's fourth argument is passed as a
  pointer; `(unsigned int)` base in address sums).
- **Wrong body or spelling**: func_001751A0 (the 0x70003A24 store runs on
  every path), func_002149F0 (read the charge once, stage the target in two
  steps), func_001F3620 (compute the reseed value before reading the mute
  byte), func_0017F5F0 (float post-decrement against a zero local),
  func_0017F320 (`if (flags) return 0; return 1;`), func_001C87C0 (absolute
  byte global, loop and lookup shapes).
- func_001E9280 was removed from fill_unmatched.py's GPREL_FORCE_ASM list:
  with the entry present it linked from the .s despite objdiff 100%; with it
  gone the boot ELF stays byte-identical and it links from C.

Parked in the second batch: func_001429D0 99.94 (prologue parameter-copy
order), func_0017C580 99.94 (one value in $a0 vs $a2, TU_STATIC tried),
func_001AD740 99.98 (v0/v1 swap), func_00209280 99.85 (two saved registers
swapped after a 5040-order declaration sweep), func_001DE920 99.22,
func_00144040 97.91 (gains only through volatile globals). Permuter runs
(10 min, 2 jobs) moved none of them.
