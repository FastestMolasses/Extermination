# Near-miss decompilations (readable C, NOT byte-identical)

This registry lists functions committed as **readable C that does not compile byte-identical**
to the original. Each is a genuine compiler artifact (register coloring, instruction scheduling,
branch-likely, frame-stride, etc.) that **no source change can fix** under the matching compiler —
the function's **logic and structure are faithful**, only the noted artifact differs.

## Why these exist

The decomp's primary goal is a maximally-faithful, byte-identical rebuild. For functions that are
genuinely wall-bound, an opaque `// INCLUDE_ASM` stub gives byte-identity but **zero readable source**
— useless as ground truth for the native-port effort. A `// NEARMISS` file instead commits the
readable, logically-faithful C **and** keeps the boot ELF byte-identical:

- The file's first line is `// NEARMISS <func> (...)`; `build.py` / `fill_unmatched.py` treat it like
  a stub — it is **not compiled, not an objdiff unit**, and **does not affect `matched_code`** (which
  continues to measure only true byte-match attempts).
- The linker fills the function's bytes from the **splat `.s`** (byte-identical), never from this C.
- Each file's header documents its **objdiff %** (best plain-C, reproducible via its `// COMPILER` /
  `// CFLAGS`) and the **precise divergence reason**.

To convert a near-miss to a true match later, reproduce the byte-exact bytes from C and drop the
`// NEARMISS` marker (it becomes a normal compiled unit at 100%).

## Registry

| Function | vram | bytes | objdiff % | compiler | divergence (compiler artifact) |
|---|---|---|---|---|---|
| func_00114360 | 0x00114360 | 228 | 89.82% | ee-gcc 2.9 | GPR coloring: `st` pointer in $v1 (orig) vs $v0 (ours); allocator deterministic |
| func_0010FAD0 | 0x0010FAD0 | 0x100 | 99.84% | ee-gcc 2.9-991111-01 | eegcc induction-variable + symbol-relocation idiom wall. 64/64 instrs match (99.84%); body |
| func_0010F7D8 | 0x0010F7D8 | 0x98 | 99.79% | ee-gcc 2.9-991111-01 | eabi frame-size stride wall: body is byte-identical (branch-likely region, all loads/store |
| func_00105390 | 0x00105390 | 0x188 | 99.39% | ee-gcc 2.9-991111-01 | eegcc branch-likely-sense wall: body byte-identical at 99.39%; sole residual is one annul  |
| func_00109F90 | 0x00109F90 | 0x3C | 99.33% | ee-gcc 2.9-991111-01 | ee-gcc register-coloring wall. Entire instruction stream, branch order (beq v1,v0 -> E68;  |
| func_001107C8 | 0x001107C8 | 0x9C | 99.33% | ee-gcc 2.9-991111-01 | ee-gcc list-scheduler artifact, NOT a permuter-crackable reg-perm. C is structurally corre |
| func_00115068 | 0x00115068 | 0xFC | 96.83% | ee-gcc 2.9-991111-01 | eegcc list-scheduler adjacent-op swap wall (deterministic, no permuter lever). Body is oth |
| func_0010DD00 | 0x0010DD00 | 0x134 | 95.95% | ee-gcc 2.9-991111-01 | eegcc coloring/list-scheduler wall. Body fully decoded and correct (DMA-chain/queue builde |
| func_00114630 | 0x00114630 | 0xD8 | 95.93% | ee-gcc 2.9-991111-01 | eegcc list-scheduler wall (confirmed s84): body byte-identical (216/216, 95.93%); two dete |
| func_00107A28 | 0x00107A28 | 0x9C | 95.77% | ee-gcc 2.9-991111-01 | ee-gcc branch-delay-slot fill choice. C is structurally correct and bytes match except one |
| func_0011DBB8 | 0x0011DBB8 | 0x2A8 | 95.41% | ee-gcc 2.9-991111-01 | eegcc constant-pool / list-scheduler wall. Function fully decompiled as fdlibm atanf (veri |
| func_00106490 | 0x00106490 | 0xB0 | 94.89% | ee-gcc 2.9-991111-01 | gcc scheduling/regalloc: single redundant 'move v0,a1' copy difference. Expected materiali |
| func_00108DB0 | 0x00108DB0 | 0xF4 | 94.23% | ee-gcc 2.9-991111-01 | eegcc GPR-coloring wall (no permuter lever, confirmed s84). Body is structurally identical |
| func_0011E748 | 0x0011E748 | 0x114 | 92.58% | ee-gcc 2.9-991111-01 | eegcc list-scheduler wall. Body byte-identical and float-register assignment correct (f20= |
| func_00113D08 | 0x00113D08 | 0x170 | 92.39% | ee-gcc 2.9-991111-01 | eegcc GPR coloring wall (no permuter lever, confirmed s84). Full CFG + instruction stream  |
| func_00114C30 | 0x00114C30 | 0xE8 | 92.36% | ee-gcc 2.9-991111-01 | eegcc o32-vs-eabi list-scheduler wall. Body decompiled correctly (sizes equal 232==232); d |
| func_001084B0 | 0x001084B0 | 0x154 | 92.31% | ee-gcc 2.9-991111-01 | eegcc coloring/reg-alloc wall (confirmed s84). Decoded to correct C (DMA setup: wait on 0x |
| func_0010AA80 | 0x0010AA80 | 0xC0 | 92.19% | ee-gcc 2.9-991111-01 | Logic/frame(0x60)/externs all correct; return type void confirmed (no trailing move v0,0). |
| func_0011E520 | 0x0011E520 | 0xFC | 91.97% | ee-gcc 2.9-991111-01 | eegcc list-scheduler wall. Exact twin of func_0011E420 (only differs: calls func_0011C128  |
| func_0011E420 | 0x0011E420 | 0xFC | 91.97% | ee-gcc 2.9-991111-01 | eegcc list-scheduler wall. Body decoded fully (64/64 instrs, correct semantics): float wra |
| func_00113478 | 0x00113478 | 0xB4 | 91.11% | ee-gcc 2.9-991111-01 | ee-gcc delay-slot-fill / instruction-scheduling permutation. Control flow, branch senses,  |
| func_00106D80 | 0x00106D80 | 0xB0 | 90.80% | ee-gcc 2.9-991111-01 | instruction-scheduling wall: list-scheduler swaps two adjacent independent ops. Expected e |
| func_00106540 | 0x00106540 | 0x1B4 | 89.90% | ee-gcc 2.9-991111-01 | eegcc coloring/list-scheduler wall. Control flow, frame size 0x80, both poll loops, and th |
| func_00117BA0 | 0x00117BA0 | 0x88 | 89.56% | ee-gcc 2.9-991111-01 | Register-allocation permutation. Size-matched (34/34 instrs); semantics fully decoded and  |
| func_00107310 | 0x00107310 | 0xF0 | 88.92% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely + list-scheduler wall. Body matches structurally. Two divergen |
| func_0011FD88 | 0x0011FD88 | 0x104 | 88.66% | ee-gcc 2.9-991111-01 | eegcc register-coloring wall. Logic fully recovered and body byte-identical APART from a s |
| func_00101BE0 | 0x00101BE0 | 0xDC | 88.36% | ee-gcc 2.9-991111-01 | eegcc reg-alloc + list-scheduler wall. Body structurally identical (88.4%). Two remaining  |
| func_00112610 | 0x00112610 | 0x148 | 87.93% | ee-gcc 2.9-991111-01 | eegcc list-scheduler + GPR-coloring wall (no permuter lever for this ee-gcc, confirmed s84 |
| func_0010CD58 | 0x0010CD58 | 0xD0 | 87.50% | ee-gcc 2.9-991111-01 | eegcc register-coloring permutation wall (permuter territory). Instruction stream is byte- |
| func_0010A400 | 0x0010A400 | 0xD4 | 87.17% | ee-gcc 2.9-991111-01 | eegcc volatile-store delay-slot-fill wall. The entire DMA-tag-build loop (slt/movz min, (c |
| func_00112C58 | 0x00112C58 | 0xC0 | 86.12% | ee-gcc 2.9-991111-01 | Near-identical twin of func_00112AD0 (only constants differ: func_00111F18(0xC), D_00241D4 |
| func_00112AD0 | 0x00112AD0 | 0xC0 | 86.12% | ee-gcc 2.9-991111-01 | branch-likely wall: expected emits a forward 'bgezl v0' on the func_0010E8A8(...) < 0 chec |
| func_0010F490 | 0x0010F490 | 0x90 | 85.69% | ee-gcc 2.9-991111-01 | Branch-likely wall (oversized +4 instrs). Two identical byte-copy loops over a struct at b |
| func_00107E88 | 0x00107E88 | 0x118 | 85.64% | ee-gcc 2.9-991111-01 | eegcc sibling-call wall. Expected emits a true tail call to func_00107CB8 (full epilogue t |
| func_00118790 | 0x00118790 | 0x98 | 85.61% | ee-gcc 2.9-991111-01 | ee-gcc register-allocation + minor store-ordering near-miss. C is semantically correct and |
| func_00119240 | 0x00119240 | 0x90 | 85.36% | ee-gcc 2.9-991111-01 | Frame-size / reg-alloc wall (off by 1 instr). Calls DisableDmacHandler(0), func_0010E088(0 |
| func_001114B8 | 0x001114B8 | 0x64 | 85.20% | ee-gcc 2.9-991111-01 | Branch-likely wall: original emits `bgezl $v0` (branch-likely) in the post-jal sign test,  |
| func_0011A070 | 0x0011A070 | 0x124 | 84.68% | ee-gcc 2.9-991111-01 | eegcc coloring + list-scheduler wall (confirmed s84, no permuter lever). The loop body is  |
| func_00108300 | 0x00108300 | 0x1B0 | 83.50% | ee-gcc 2.9-991111-01 | eegcc list-scheduler wall. Body matches 100% from RemoveIntcHandler onward; only the prolo |
| func_00118078 | 0x00118078 | 0x134 | 81.88% | ee-gcc 2.9-991111-01 | eegcc GPR-coloring wall. Fully decompiled: a loop over the table at D_0027CCC0 (stride 0x6 |
| func_0010CE28 | 0x0010CE28 | 0xBC | 81.64% | ee-gcc 2.9-991111-01 | Logic/frame/externs all correct (store-reload of D_00276E10 reproduced via volatile read;  |
| func_00109E68 | 0x00109E68 | 0x124 | 80.71% | ee-gcc 2.9-991111-01 | eegcc coloring + address-rematerialization wall. Decoded fully (control flow + semantics c |
| func_001186B0 | 0x001186B0 | 0xDC | 79.67% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely wall (confirmed s84). Same weapon-table scan family as func_00 |
| func_001129E8 | 0x001129E8 | 0xE4 | 79.49% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely wall (confirmed s84): body matches; sole blocker is the func_0 |
| func_001063E8 | 0x001063E8 | 0xA4 | 78.83% | ee-gcc 2.9-991111-01 | ee-gcc regalloc/rematerialization wall. Structure (guard + do-while watchdog poll loop on  |
| func_00109FD0 | 0x00109FD0 | 0x170 | 78.07% | ee-gcc 2.9-991111-01 | eegcc coloring/reg-alloc wall (no permuter lever, confirmed s84). Full control flow + bran |
| func_001118B8 | 0x001118B8 | 0x94 | 77.30% | ee-gcc 2.9-991111-01 | gcc-2.9 branch-shape + delay-slot-fill wall. Frame matches (0x50) and call/store data flow |
| func_00110508 | 0x00110508 | 0x100 | 77.28% | ee-gcc 2.9-991111-01 | eegcc list-scheduler + loop-rotation tail-duplication + GPR-coloring wall. Body logic full |
| func_001190B8 | 0x001190B8 | 0x138 | 76.26% | ee-gcc 2.9-991111-01 | eegcc global-address %hi-caching / coloring wall. Body fully decoded and correct (init: fu |
| func_00110048 | 0x00110048 | 0x104 | 76.20% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely wall (confirmed s84) + frame-stride wall. Body fully recovered |
| func_00110198 | 0x00110198 | 0xEC | 75.81% | ee-gcc 2.9-991111-01 | eegcc arg-passing-ABI + forward-branch-likely wall. Function body is structurally identica |
| func_00112B90 | 0x00112B90 | 0xC4 | 75.67% | ee-gcc 2.9-991111-01 | Branch-likely wall: byte-identical through offset 0x74; sole divergence is expected bgezl  |
| func_00113A58 | 0x00113A58 | 0xF8 | 74.68% | ee-gcc 2.9-991111-01 | eegcc frame-stride + global GPR-coloring wall (no permuter lever). Three independent facto |
| func_001115D0 | 0x001115D0 | 0xB0 | 74.52% | ee-gcc 2.9-991111-01 | ee-gcc address-base register-allocation + reload-scheduling permutation. Logic/structure c |
| func_0010EFD0 | 0x0010EFD0 | 0x140 | 73.67% | ee-gcc 2.9-991111-01 | eegcc hand-coded-asm delay-loop wall. Fully decompiled: calls func_0010E088(0); retry-loop |
| func_0010F870 | 0x0010F870 | 0x88 | 73.56% | ee-gcc 2.9-991111-01 | Busy-wait delay loop in target is emitted as lui %hi(D_FFFFF); addiu %lo(D_FFFFF) using li |
| func_00102530 | 0x00102530 | 0xD0 | 73.15% | ee-gcc 2.9-991111-01 | eegcc regalloc + inner-loop idiom near-miss. Clean -O2 C reaches 73% (permuter plateaued ~ |
| func_00110368 | 0x00110368 | 0x130 | 71.57% | ee-gcc 2.9-991111-01 | eegcc global-address %hi-caching / coloring wall (+ loop branch-likely shape). Body fully  |
| func_00102468 | 0x00102468 | 0xC4 | 70.76% | ee-gcc 2.9-991111-01 | Same frame-size stride wall as func_001021B0 (identical spin-wait pattern with mode/timeou |
| func_00111950 | 0x00111950 | 0x68 | 70.38% | ee-gcc 2.9-991111-01 | Body is logically byte-correct (same instruction count, same relocs, same control flow: if |
| func_00107098 | 0x00107098 | 0xE0 | 70.09% | ee-gcc 2.9-991111-01 | eegcc register-allocation near-miss (permuter territory). Body logic decompiled correctly  |
| func_00117CB0 | 0x00117CB0 | 0x64 | 70.00% | ee-gcc 2.9-991111-01 | Body decoded correctly and compiles at 70%. Two residual diffs are genuine ee-gcc EE-codeg |
