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
| func_001021B0 | 0x001021B0 | 0xC4 | 69.98% | ee-gcc 2.9-991111-01 | Frame-size stride wall: correct readable C (DMA/IRQ spin-wait; loop body, srl, li constant |
| func_00101CD0 | 0x00101CD0 | 0x1D8 | 69.44% | ee-gcc 2.9-991111-01 | MMIO config writer: validation branches, the 0xe000 bit-twiddle (D_002410B8/C8/D8 table lo |
| func_001176E0 | 0x001176E0 | 0x104 | 68.60% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely wall + list-scheduler wall. Logic fully recovered: scan 0x30 e |
| func_0011D878 | 0x0011D878 | 0x294 | 66.98% | ee-gcc 2.9-991111-01 | eegcc float-constant codegen wall. Function is fdlibm __kernel_tanf(float x,float y,int iy |
| func_00101F08 | 0x00101F08 | 0xD8 | 66.39% | ee-gcc 2.9-991111-01 | eegcc degenerate dead-backedge codegen wall (NOT the branch-likely wall). The terminal bne |
| func_00119C98 | 0x00119C98 | 0x6C | 65.19% | ee-gcc 2.9-991111-01 | ee-gcc reg-alloc/delay-slot wall. C reaches objdiff 96.3% (permuter best, score 100); body |
| func_00101FE0 | 0x00101FE0 | 0xE4 | 64.70% | ee-gcc 2.9-991111-01 | Two stacked compiler-internal walls, both non-permutable. (1) Degenerate do{}while(0) inne |
| func_0010F3B0 | 0x0010F3B0 | 0xE0 | 64.12% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely wall (confirmed s84). Decoded as a wrapper: s0=func_0010EFA8() |
| func_00108818 | 0x00108818 | 0x88 | 63.06% | ee-gcc 2.9-991111-01 | gcc-2.9 emits movn (conditional move) for the (hi==0xFF00<<24)?0x20:0 sh-selection; target |
| func_00114708 | 0x00114708 | 0x140 | 63.02% | ee-gcc 2.9-991111-01 | Semantics fully decoded and compile correct (D_00241D68 guard, D_0027B0C0->0x24 check retu |
| func_001020C8 | 0x001020C8 | 0xE4 | 62.98% | ee-gcc 2.9-991111-01 | eegcc dead-single-iteration-loop shape + regalloc wall: body decoded correctly (flag-poll  |
| func_00102368 | 0x00102368 | 0xF0 | 61.92% | ee-gcc 2.9-991111-01 | eegcc regalloc + dead-loop-codegen wall. Branch-likely actually matched here (our `bnel v1 |
| func_001172B8 | 0x001172B8 | 0x170 | 61.66% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely wall (confirmed s84). The two table-scan loops are built entir |
| func_00106830 | 0x00106830 | 0x114 | 60.94% | ee-gcc 2.9-991111-01 | eegcc register-hoist / frame-stride wall. Logic recovered but our ee-gcc 2.9-991111-01 all |
| func_00110608 | 0x00110608 | 0x84 | 60.70% | ee-gcc 2.9-991111-01 | ee-gcc reg-alloc + frame-size wall. Correct-frame (0x110) C reaches objdiff 86.3% (permute |
| func_0010A248 | 0x0010A248 | 0x4C | 57.26% | ee-gcc 2.9-991111-01 | ee-gcc list-scheduler permutation wall. Reg-alloc and all instructions match the target ex |
| func_0010F9E0 | 0x0010F9E0 | 0xF0 | 57.15% | ee-gcc 2.9-991111-01 | eegcc codegen-shape wall (confirmed). C is semantically correct and reaches 57.15% best at |
| func_001177E8 | 0x001177E8 | 0xD8 | 56.35% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely wall (confirmed s84). Expected emits bnel/beqzl/bnezl on the n |
| func_0010E6F8 | 0x0010E6F8 | 0x11C | 56.27% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely + return-value coloring wall. Two non-source-crackable artifac |
| func_0010E4C0 | 0x0010E4C0 | 0x134 | 56.21% | ee-gcc 2.9-991111-01 | Body byte-identical at correct 0x90 frame, but two deterministic ee-gcc 2.9 codegen select |
| func_00119EA0 | 0x00119EA0 | 0x1CC | 56.05% | ee-gcc 2.9-991111-01 | eegcc forward-branch-likely wall (confirmed s84). The inner slot-scan loop (.L00119FD8) re |
| func_00117088 | 0x00117088 | 0x22C | 55.16% | ee-gcc 2.9-991111-01 | eegcc coloring/reg-alloc wall (confirmed s84). Function fully decoded to logically-correct |
| func_00102278 | 0x00102278 | 0xEC | 54.73% | ee-gcc 2.9-991111-01 | eegcc multi-wall (o32-vs-eabi regalloc + dead-loop idiom + frame stride). Decompiled seman |
| func_0011A2B0 | 0x0011A2B0 | 0x1C0 | 53.72% | ee-gcc 2.9-991111-01 | eegcc frame-stride + coloring wall. Expected frame is 0x50 (4 callee-saved: keeps the 0xFF |
| func_00112E28 | 0x00112E28 | 0x170 | 51.49% | ee-gcc 2.9-991111-01 | Structurally identical sibling of func_00111F18 (semaphore re-init, different data symbols |
| func_00111F18 | 0x00111F18 | 0x170 | 51.49% | ee-gcc 2.9-991111-01 | Two confirmed-s84 ee-gcc walls. (1) eegcc forward-branch-likely wall: expected emits bgezl |
| func_00106948 | 0x00106948 | 0x168 | 50.74% | ee-gcc 2.9-991111-01 | eegcc reg-alloc wall / saved-register-count divergence (confirmed s84). Instruction-for-in |
| func_001B11E0 | 0x001B11E0 | 0x58 | 79.32% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation/scheduling permutation: identical opcodes (lbu/sra/sll/andi/lui/addiu/ |
| func_001F3FA0 | 0x001F3FA0 | 0x64 | 72.60% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | loop-invariant-%hi-CSE / addressing-mode: target recomputes 'lui at,%hi(D_004E1340); addu  |
| func_001236D8 | 0x001236D8 | 0x78 | 87.83% | ee-gcc 2.9-991111-01 | ee-gcc function (target uses sd/ld 64-bit callee saves, daddu reg moves, and a bnezl branc |
| func_001D2090 | 0x001D2090 | 0x7C | 97.10% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation-ORDER wall (first half). Residual is 10 DIFF_ARG_MISMATCH only: instru |
| func_001EBE90 | 0x001EBE90 | 0x78 | 79.80% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | instruction-scheduling wall. Residual is 3 DIFF_INSERT / 3 DIFF_DELETE of identical instru |
| func_001EACF0 | 0x001EACF0 | 0x7C | 80.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant scheduling permutation: target hoists the multi-instruction 1e-6f const (lui+o |
| func_001EAF00 | 0x001EAF00 | 0x7C | 80.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Identical wall to func_001EACF0 (same template, const 5.0f, table D_002557D0, flag 1): FP- |
| func_001EBBB0 | 0x001EBBB0 | 0x7C | 80.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Identical wall to func_001EACF0 (same template, const 3.0f, table D_002561F0, flag 1): FP- |
| func_001EBE10 | 0x001EBE10 | 0x7C | 80.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant emit-order / instruction-scheduling permutation: CW target emits the f15(1e-6f |
| func_001281C0 | 0x001281C0 | 0x8C | 78.29% | ee-gcc 2.9-991111-01 | ee-gcc codegen function (not mwcc): boot uses sd/ld $ra, xori-equality, sltiu, movn. Body  |
| func_001D9EE0 | 0x001D9EE0 | 0x194 | 99.75% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 99.75% mwcc 2.3.3 (-sdatathreshold 4). Body byte-identical; sole residual is a register-co |
| func_001EF780 | 0x001EF780 | 0x1B4 | 97.60% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register coloring / paired-single allocation + order of materializing the two call-argu |
| func_001F6BB0 | 0x001F6BB0 | 0x1A8 | 78.73% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW-vs-mwcc branch lowering (NOT logic): (1) original CodeWarrior left an unfilled nop in t |
| func_001D8690 | 0x001D8690 | 0x220 | 97.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body fully correct (gp-rel reload-per-access reproduced, sized-array non-gp 0x28 symbols). |
| func_001D89D0 | 0x001D89D0 | 0x21C | 97.63% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body fully correct: 5-arg func_001D8340 (pointer as 5th arg) per matched sibling func_001D |
| func_001FA330 | 0x001FA330 | 0x234 | 95.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | regalloc |
| func_001EF510 | 0x001EF510 | 0x26C | 98.63% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | instruction-scheduling permutation: the target reverses the f14(1.0f)/f15(9.99e-7) immedia |
| func_001E7050 | 0x001E7050 | 0x2B4 | 97.25% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation-order (coloring) permutation: target colors src->s1 and the 0x1F0 tabl |
| func_0012D580 | 0x0012D580 | 0x2C4 | 96.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | branch-delay-slot scheduling permutation: target (CW 2.3.1) leaves explicit NOPs in the sw |
| func_001FF3F0 | 0x001FF3F0 | 0x19C | 87.94% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 87.9% mwcc 2.3.3 (-sdatathreshold 4). Body/4-state control flow byte-correct (gp-rel point |
| func_001FF1E0 | 0x001FF1E0 | 0x210 | 82.06% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body fully correct (resource-bank streamer): gp-rel D_00275C70 vs non-gp 0x28xxxx symbols  |
| func_0018B9C0 | 0x0018B9C0 | 0x25C | 84.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + branch-likely-lowering scheduling permuter class. Logic fully recovere |
| func_001FA0D0 | 0x001FA0D0 | 0x260 | 92.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring / store-scheduling permuter class. Logic fully recovered. Two artifacts  |
| func_0017F320 | 0x0017F320 | 0x2C4 | 97.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Pure delay-slot-fill / scheduling permuter class. Logic fully recovered (97.6%). The three |
| func_0017C580 | 0x0017C580 | 0x2D4 | 98.78% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 98.78% near-miss, logic fully recovered. 2 residual instrs: (1) register-allocation permut |
| func_00191D40 | 0x00191D40 | 0x2C4 | 91.27% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 91.27% near-miss, logic fully recovered and FP register coloring (fparg1=f21, delta=f20, f |
| func_001F9820 | 0x001F9820 | 0x300 | 95.35% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling permutation in channels 3 and 4: mwcc places the D_00275B24/B20 sou |
| func_001EEBA0 | 0x001EEBA0 | 0x30C | 97.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two permuter-class residuals (NOT the clean-store nop): (1) the middle of three identical  |
| func_001EEEB0 | 0x001EEEB0 | 0x30C | 97.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Identical residual class to its twin func_001EEBA0 (NOT the clean-store nop): middle-itera |
| func_001FBF50 | 0x001FBF50 | 0x328 | 99.87% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring + 2-instruction delay-slot scheduling near-miss. Body/structure fully |
| func_001FA790 | 0x001FA790 | 0x328 | 89.50% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-ORDER permutation. Body and control flow fully recovered and logically |
| func_0017B910 | 0x0017B910 | 0x330 | 80.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Branch-likely scheduling + FP register-coloring near-miss. Body and control flow fully rec |
| func_0012D240 | 0x0012D240 | 0x33C | 98.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | State-dispatch register coloring + branch-likely lowering + addu operand commutation; see  |
| func_001ED450 | 0x001ED450 | 0x348 | 94.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant register-coloring permutation in the func_001CFB50 call setup. Body/structure  |
| func_001ED7A0 | 0x001ED7A0 | 0x34C | 95.73% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant register-coloring permutation in the func_001CFB50 call setup -- same class an |
| func_001EDAF0 | 0x001EDAF0 | 0x34C | 82.52% | mwcc 2.3 (mwcps2-2.3-991202) | FP-constant emit-order + float-div/int-mul instruction scheduling + FP register-coloring p |
| func_001EDE40 | 0x001EDE40 | 0x34C | 82.52% | mwcc 2.3 (mwcps2-2.3-991202) | Body and structure fully recovered (objdiff 82.52% via mwcc 991202 -O4,p -sdatathreshold 4 |
| func_001EE190 | 0x001EE190 | 0x350 | 83.68% | mwcc 2.3 (mwcps2-2.3-991202) | Body and structure fully recovered (objdiff 83.68% via mwcc 991202 -O4,p -sdatathreshold 4 |
| func_001EE4E0 | 0x001EE4E0 | 0x350 | 85.78% | mwcc 2.3 (mwcps2-2.3-991202) | see above |
| func_001EF1C0 | 0x001EF1C0 | 0x350 | 96.57% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring permutation in the func_001CFB50 call setup. Body/structure/init-bloc |
| func_001EE830 | 0x001EE830 | 0x364 | 95.88% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring permutation in the func_001CFB50 call setup, identical to sibling fun |
| func_001EF9D0 | 0x001EF9D0 | 0x350 | 88.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Integer register-allocation/coloring permutation. Logic, control flow, both switch dispatc |
| func_001D91A0 | 0x001D91A0 | 0x388 | 94.05% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation (permuter territory): callee-saved coloring of the 4 |
| func_001DA310 | 0x001DA310 | 0x384 | 93.21% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling permutation (permuter territory). Body+control flow fully r |
| func_0018ABA0 | 0x0018ABA0 | 0x3A4 | 99.57% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-compare delay-slot scheduling: target speculates `addiu a0,s0,0xA0` (else-arm call arg) |
| func_001509A0 | 0x001509A0 | 0x400 | 96.48% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP even/odd register-coloring permutation: in the three easing tails the target pairs the  |
| func_0012B410 | 0x0012B410 | 0x434 | 98.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring near-miss (98.31% on mwcc 2.3.3; logic fully recovered). Body, the 5-way |
| func_00156F30 | 0x00156F30 | 0x430 | 91.50% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-register-half coloring + scheduling near-miss (91.50% on mwcc 2.3.3; logic fully recove |
| func_0015FDF0 | 0x0015FDF0 | 0x42C | 91.01% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | see above |
| func_001F88C0 | 0x001F88C0 | 0x46C | 93.45% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation + FP-register coloring permutation. Body and control flow are byte-ide |
| func_001776E0 | 0x001776E0 | 0x494 | 75.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction scheduling + tail lowering. mwcc hoists the 4.0f constant load before the scra |
| func_001F8350 | 0x001F8350 | 0x484 | 92.70% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation in the particle loop: the four loop induction values (LCG  |
| func_001F91C0 | 0x001F91C0 | 0x494 | 69.62% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Stack-frame layout + callee-saved regalloc permutation. mwcc hoists several &v[N] field-ad |
| func_00174AC0 | 0x00174AC0 | 0x508 | 98.48% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | branch-likely-sense: st==7 dispatch emits plain `bne` vs target `beql` (idiom-20 likely-br |
| func_00151940 | 0x00151940 | 0x504 | 97.27% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Scheduling/regalloc artifacts: (1) `*-1.0f` negate emitted with commuted mul operands (mul |
| func_001F7E40 | 0x001F7E40 | 0x508 | 55.17% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling/regalloc divergence across the 32-iter inner loop: target keeps the |
| func_00160220 | 0x00160220 | 0x5A4 | 99.81% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-register-coloring permutation in the D_00810700==1 range-check arm; the other two mode  |
| func_001549C0 | 0x001549C0 | 0x540 | 63.95% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Callee-saved register-allocation permutation. The target colors the SCAN loop counter -> $ |
| func_001F77B0 | 0x001F77B0 | 0x540 | 77.13% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Callee-saved register-allocation permutation. The persistent work pointer (s0+0x1F0) lands |
| func_0012F100 | 0x0012F100 | 0x5BC | 91.68% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Scheduling/regalloc permutation. Body fully recovered and structurally byte-identical (367 |
| func_0014FBC0 | 0x0014FBC0 | 0x5CC | 72.16% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Scheduling/regalloc permutation. Logic fully recovered. The 991202+target leave the clean  |
| func_001D98A0 | 0x001D98A0 | 0x638 | 83.80% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FPU-MAC (3-term dot-product no madd fusion) + shared-0.0f FP constant coloring in callee-s |
| func_00196CE0 | 0x00196CE0 | 0x6B0 | 95.32% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation / rematerialization + call-arg-register scheduling (permuter class), N |
| func_0012CAA0 | 0x0012CAA0 | 0x7A0 | 98.50% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Permuter-class register/scheduling artifacts on an otherwise byte-faithful body (3 residua |
| func_001809B0 | 0x001809B0 | 0x760 | 83.35% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Permuter-class register-coloring + FP-arg scheduling on a fully-recovered body. Residual c |
| func_00156620 | 0x00156620 | 0x910 | 95.01% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 95.0% (mwcc233) near-miss, logic fully recovered. Residuals are NOT the clean-store nop: 4 |
| func_001EC1F0 | 0x001EC1F0 | 0x7C | 76.87% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body 100% correct (93.5% mwcc991202). Two residuals, both pure scheduling artifacts no C c |
| func_001EC3F0 | 0x001EC3F0 | 0x7C | 80.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant emit-order/instruction-scheduling permutation. CW target emits f15(1e-6f, 2-in |
| func_001EC9A0 | 0x001EC9A0 | 0x7C | 80.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant emit-order/instruction-scheduling permutation (same class as func_001EC3F0/fun |
| func_001ECE70 | 0x001ECE70 | 0x7C | 80.58% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant emit-order/instruction-scheduling permutation (same class as func_001EC3F0/fun |
| func_001D4DA0 | 0x001D4DA0 | 0x80 | 98.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation: instruction structure byte-identical to confirmed sibling |
| func_001FE920 | 0x001FE920 | 0x7C | 92.74% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Calling-convention/regalloc permutation: control flow, 0x20 frame with s0 save, and switch |
| func_001D4F30 | 0x001D4F30 | 0x80 | 82.50% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation + scheduling permutation: loop structure, the min(t1,0x1f8) clamp, and |
| func_00128B80 | 0x00128B80 | 0x84 | 83.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW-vs-mwcc branch lowering / dead trailing return-0 block. Body, stores, call, and the con |
| func_001AA840 | 0x001AA840 | 0x84 | 84.27% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation wall: live-across-call values in caller-saved temps vs mwcc's callee-s |
| func_001AF800 | 0x001AF800 | 0x84 | 59.06% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two compounding 2.3.1 artifacts neither 991 nor 233 fixes (991=73.42% best; 233 regresses  |
| func_001FECB0 | 0x001FECB0 | 0x84 | 84.70% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Regalloc/constant-rematerialization permutation: target keeps compare constant 1 live in v |
| func_001FE9E0 | 0x001FE9E0 | 0x88 | 78.53% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Constant-rematerialization + branch-sense permutation: target re-emits li v0,1 inside each |
| func_00173DD0 | 0x00173DD0 | 0x8C | 94.69% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | reloc-pair interleave: CW materializes &D_002486F0 as split lui/addiu/lq (full GPR address |
| func_00228320 | 0x00228320 | 0x8C | 76.63% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation / instruction-scheduling: CW keeps the 3-int stack temp via dead-store |
| func_0017F130 | 0x0017F130 | 0x90 | 84.72% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc-vs-CW branch lowering + FP move order: CW lowers the mode-4 `return func()!=0` as an  |
| func_00183910 | 0x00183910 | 0x90 | 94.58% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-coloring / delay-slot scheduling: CW keeps arg0 in $a0 across the dispatch and co |
| func_00194D10 | 0x00194D10 | 0x94 | 84.73% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-compare branch-lowering wall + arg-eval scheduling. Logic fully recovered. Residual 1:  |
| func_001D4B80 | 0x001D4B80 | 0x98 | 77.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation: body/structure byte-identical to confirmed sibling func_0 |
| func_001D4C30 | 0x001D4C30 | 0x98 | 77.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation (identical to func_001D4B80, differs only in bank base D_0 |
| func_0019AA80 | 0x0019AA80 | 0x9C | 90.51% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | branch-delay-slot scheduling: target keeps nop in the beqz delay slot, both 991202 and 233 |
| func_001AF220 | 0x001AF220 | 0x9C | 76.87% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | constant-propagation era difference: target reloads (lbu) D_00810700/D_00810701 before the |
| func_001CB2C0 | 0x001CB2C0 | 0x9C | 83.72% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | instruction-scheduling permutation: target emits `addiu s1,a2,0x10` before `sw zero,0x10(a |
| func_001AB6A0 | 0x001AB6A0 | 0xA0 | 90.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW-vs-mwcc branch lowering + delay-slot address hoist. Two residuals: (1) mwcc fills the ` |
| func_001B32F0 | 0x001B32F0 | 0xA0 | 91.88% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Delay-slot move scheduling on the first call result. Entire tail (3 if-guards, c.le.s test |
| func_001B65C0 | 0x001B65C0 | 0xA0 | 88.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register allocation: caller-saved temp held across a call. Body/structure fully match (gp_ |
| func_001D7100 | 0x001D7100 | 0xA0 | 98.88% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Pure register-allocation permutation: all instructions + schedule identical, only GPR name |
| func_001DAFA0 | 0x001DAFA0 | 0xA0 | 91.88% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Float-store body (bulk) fully matches. Loop-preamble residual: register coloring + mwcc re |
| func_0013D220 | 0x0013D220 | 0xA4 | 98.41% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Single branch delay-slot scheduling difference: target emits 'bnez v0,0x74; nop' (empty sl |
| func_001B3440 | 0x001B3440 | 0xA4 | 92.07% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Delay-slot move scheduling on first call result: CW tests raw return reg (bnez v0) with `p |
| func_001F4010 | 0x001F4010 | 0xA8 | 92.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring permutation. Full logic recovered; needs -sdatathreshold 8 (D_00275C40 m |
| func_001762E0 | 0x001762E0 | 0xAC | 94.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling / pointer-copy-direction artifact. Full logic recovered (hoisting t |
| func_001D7A80 | 0x001D7A80 | 0xAC | 57.42% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation (permuter territory). Body fully recovered and struc |
| func_00121870 | 0x00121870 | 0xB0 | 60.48% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Hand-written-assembly memcpy: register coloring (immutable dst base in t0, running ptrs v1 |
| func_001AB430 | 0x001AB430 | 0xB0 | 82.16% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling near-miss: target hoists shared lui %hi(D_00810118) and keeps sourc |
| func_001AB4E0 | 0x001AB4E0 | 0xB0 | 88.64% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Constant-materialization + register-coloring near-miss: target keeps the masked-0 (andi a2 |
| func_001A8F40 | 0x001A8F40 | 0xBC | 85.21% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-coloring/scheduling permutation: target reloads *0x70003A20 early into $f1 and colors v |
| func_001B0EA0 | 0x001B0EA0 | 0xC0 | 95.83% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Prologue instruction-scheduling artifact: body 100% identical except one instruction's pos |
| func_001D7FA0 | 0x001D7FA0 | 0xC0 | 97.29% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-save / prologue scheduling-order permutation: target saves both callee fprs (f12- |
| func_001AE5E0 | 0x001AE5E0 | 0xC4 | 89.35% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body fully recovered. Sole residual is a pure list-scheduler permutation of the two indepe |
| func_001D37D0 | 0x001D37D0 | 0xC4 | 95.41% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body fully recovered (reload idiom #10 applied: *slot is reloaded before each of the 4 fie |
| func_00180460 | 0x00180460 | 0xC8 | 99.80% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-register-coloring on the commutative add.s. Target loads *(float*)0x700038A4 into $f1 a |
| func_0021D530 | 0x0021D530 | 0xC4 | 98.67% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Delay-slot scheduling (inverse of the clean-store-nop case). The original CodeWarrior buil |
| func_00206BF0 | 0x00206BF0 | 0xC8 | 97.40% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Single-store addressing-mode/scheduling artifact: the CW target materializes &v34 into a r |
| func_001AF5C0 | 0x001AF5C0 | 0xCC | 81.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Store-scheduling artifact: source order already matches the CW target exactly (C4,B2, floa |
| func_00205740 | 0x00205740 | 0xCC | 94.90% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling near-miss (94.9%, mwcc233): logic fully recovered, identical instru |
| func_001D7510 | 0x001D7510 | 0xD0 | 57.73% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling near-miss (57.7%, mwcc233): logic fully recovered, identica |
| func_001FAFD0 | 0x001FAFD0 | 0xD8 | 70.93% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Branch-lowering + delay-slot-scheduling permutation: distinct early-return epilogue (ra-re |
| func_001A8DA0 | 0x001A8DA0 | 0xDC | 87.82% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling / delay-slot-fill permutation only (register coloring s0=e,s1=p,s2= |
| func_001D64A0 | 0x001D64A0 | 0xDC | 90.09% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring (target colors address/const temporaries $t0-$t4; mwcc picks  |
| func_001FCE30 | 0x001FCE30 | 0xE0 | 74.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure/addressing/both dispatch arms byte-identical. Sole residual is the packed-p |
| func_00203E60 | 0x00203E60 | 0xE0 | 91.00% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/control-flow/param-mapping (leading unused arg, real params $a1/$a2)/addressing (0x50 |
| func_00157F60 | 0x00157F60 | 0xE4 | 96.75% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation-order permutation: body 100% correct (all opcodes/order match), sole r |
| func_0017FB90 | 0x0017FB90 | 0xE4 | 99.82% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-register-coloring permutation: single commutative add.s where target uses the pi/2 cons |
| func_001CEEE0 | 0x001CEEE0 | 0xE4 | 83.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | prologue instruction-scheduling + 128-bit-load register coloring: logic fully recovered an |
| func_001EBC30 | 0x001EBC30 | 0xE4 | 73.30% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant emit-order / instruction-scheduling permutation. Body 100% logically correct,  |
| func_001EBD20 | 0x001EBD20 | 0xE4 | 73.30% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant emit-order / instruction-scheduling permutation, identical to func_001EBC30 (s |
| func_001FD6A0 | 0x001FD6A0 | 0xE4 | 60.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation / loop-induction-variable permutation. Instruction content matches (sa |
| func_0019B7D0 | 0x0019B7D0 | 0xE8 | 92.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Scheduler delay-slot-fill artifact (mwcc 2.3.3 vs 2.3.1 target). 233 fills the `beqz s0` b |
| func_001D4650 | 0x001D4650 | 0xE8 | 88.34% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + instruction-scheduling permutation: 88.34% on 2.3.3, body/values fully |
| func_001D63B0 | 0x001D63B0 | 0xE8 | 95.78% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-ORDER wall. Body + structure are byte-exact under mwcc 2.3.3 (zero ins |
| func_001AFD70 | 0x001AFD70 | 0xEC | 97.46% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Saved-register-allocation-ORDER wall (identical residual under 991202 and 2.3.3). Entire b |
| func_001BA7F0 | 0x001BA7F0 | 0xEC | 99.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | jal arg-setup / delay-slot scheduling order. Logic + structure are byte-exact under mwcc 2 |
| func_00213F30 | 0x00213F30 | 0xF0 | 99.33% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring permutation: hoisted fill const 0x6D and the loop's 0x18 counter swap be |
| func_001D6BA0 | 0x001D6BA0 | 0xEC | 90.34% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation cascade (buffer base $t0 vs $a3) + scheduling order of the +0x4 header |
| func_001575E0 | 0x001575E0 | 0xF4 | 78.93% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling permutation: target reuses $v0 for the three scratchpad flo |
| func_0018C5A0 | 0x0018C5A0 | 0xF4 | 92.21% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Branch-likely SENSE (target bc1tl vs mwcc bc1fl) on the min-clamp, plus FP register colori |
| func_00197390 | 0x00197390 | 0xF4 | 96.30% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling / delay-slot-fill: (1) the func_00102948 call sets its two args in  |
| func_001A8BE0 | 0x001A8BE0 | 0xF4 | 94.59% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling / delay-slot-fill artifact: target emits the two init loads (D_0027 |
| func_001D6E60 | 0x001D6E60 | 0xF8 | 84.68% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation: target materialises constants into temporaries t0-t |
| func_0017C440 | 0x0017C440 | 0xFC | 96.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW-vs-mwcc branch-delay-slot scheduling: target leaves the `bne v1,v0` (idx==2 selector) d |
| func_00187CC0 | 0x00187CC0 | 0x100 | 88.95% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation + instruction-scheduling permutation (logic fully recovered, structure |
| func_001221E0 | 0x001221E0 | 0x100 | 87.08% | ee-gcc 2.9-991111-01 | ee-gcc register-allocation permutation (NOT mwcc; target uses sd/ld/daddu EABI saves -> co |
| func_001D2730 | 0x001D2730 | 0x100 | 95.12% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring permutation: target colors bit->s1/flags->s0/hit->s2, mwcc picks bit->s0 |
| func_001D7410 | 0x001D7410 | 0x100 | 99.14% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring permutation of the two loop vars: target e->s1/off->s2 with field temp i |
| func_00206A00 | 0x00206A00 | 0x100 | 98.66% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling permutation: target sinks a0->s5 save into the first call's delay s |
| func_00179450 | 0x00179450 | 0x104 | 99.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register coloring: target keeps `thr` (the +0xB4 field) in $f1 (fv0f) but mwcc 2.3.3 co |
| func_0019B6C0 | 0x0019B6C0 | 0x104 | 81.83% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation ordering in the two 3-element copy loops: the target assigns the loop  |
| func_001C5570 | 0x001C5570 | 0x108 | 98.33% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-materialization artifact: target holds a 16-bit-masked zero (andi v0,zero,0xffff) |
| func_00124F58 | 0x00124F58 | 0x10C | 84.03% | ee-gcc 2.9-991111-01 | EE-GCC function, not mwcc: disasm has daddu register moves, sd/ld 8-byte saves, and pervas |
| func_001FEE60 | 0x001FEE60 | 0x10C | 96.69% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation permutation: body/control-flow/gp-rel all byte-faithful (96.69% mwcc23 |
| func_0021E9C0 | 0x0021E9C0 | 0x10C | 96.87% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-coloring + constant-CSE artifact: target uses call-clobbered a1 for the switch-di |
| func_0014A620 | 0x0014A620 | 0x110 | 96.76% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation/scheduling artifact. Body 100% logically correct (3-way state dispatch |
| func_001CABA0 | 0x001CABA0 | 0x110 | 93.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation/scheduling artifact. Body 100% logically complete & faithful. Keys tha |
| func_001D3E40 | 0x001D3E40 | 0x110 | 71.07% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation/scheduling artifact. Body logically complete & faithful (verified op-f |
| func_001284E0 | 0x001284E0 | 0x114 | 99.86% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring (permuter-class): the < 0xB guard's slti destination is $v0 ( |
| func_00145880 | 0x00145880 | 0x114 | 99.57% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-register coloring (permuter-class): several reloaded float temps land in $f2/$ft0 vs ta |
| func_002081A0 | 0x002081A0 | 0x110 | 50.85% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Pervasive register-allocation coloring + mwcc list-scheduling of the GIF-tag constant comp |
| func_0014AE80 | 0x0014AE80 | 0x114 | 99.54% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring permutation: target carries the decayed timer t in $f0 and freshly-lo |
| func_00188430 | 0x00188430 | 0x114 | 82.04% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Three stacked artifacts: (1) GPR coloring swap arg0=$s1/n=$s0 (mwcc emits $s0/$s1, cascade |
| func_001A9360 | 0x001A9360 | 0x114 | 92.45% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring + load-scheduling permutation around the v/sq compute: target loads t |
| func_001CA0A0 | 0x001CA0A0 | 0x114 | 99.06% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body byte-identical (insns 7-68). Sole residual: clamp branch delay-slot fill. Target leav |
| func_001D21E0 | 0x001D21E0 | 0x114 | 70.78% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Head (dma_wait_and_submit block, insns 18-22) and tail (func_00101F08 with field_9C<<14, i |
| func_001F9660 | 0x001F9660 | 0x114 | 96.23% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | All logic, case constants, store values, branch-likely (beql) dispatch, and single shared  |
| func_0015C1F0 | 0x0015C1F0 | 0x118 | 80.87% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Inner 3-way select (sel 0/2/1) dispatch-shape / branch-scheduling permutation: mwcc emits  |
| func_00189EC0 | 0x00189EC0 | 0x118 | 73.00% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | switch case-body layout / branch-lowering permutation: target places the return-1/return-2 |
| func_001FD580 | 0x001FD580 | 0x118 | 91.19% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation: instruction stream is 1:1 with the target (same opcodes,  |
| func_001AE7E0 | 0x001AE7E0 | 0x120 | 99.10% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Single delay-slot scheduling artifact: mwcc233 hoists the raw-constant `lui at,0x7000` (ab |
| func_001B2E50 | 0x001B2E50 | 0x120 | 86.39% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Compiler-version guard-elision + register-allocation permutation. Target (mwcc 2.3.1.01) o |
| func_0019A440 | 0x0019A440 | 0x124 | 83.41% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | GPR register-coloring permutation: target colors ret=$s0 / buf-base=$s1, my build swaps to |
| func_001D9070 | 0x001D9070 | 0x124 | 96.07% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Pure register-coloring permutation (FPR + GPR). After splitting the inner loop into two pa |
| func_0022A460 | 0x0022A460 | 0x124 | 82.25% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + scheduling permutation on a recovered nested state-machine switch. Rew |
| func_001E7310 | 0x001E7310 | 0x128 | 99.11% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register coloring: entire float block uniformly shifted ~2 FP registers (target uses f1 |
| func_001E7440 | 0x001E7440 | 0x128 | 99.11% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register coloring: same uniform ~2 FP-register shift as func_001E7310 (target f1/f2/f4  |
| func_00181F60 | 0x00181F60 | 0x128 | 94.46% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two compiler artifacts: (1) instruction scheduling -- mwcc 2.3.3 emits jal func_0019A570 b |
| func_001E7570 | 0x001E7570 | 0x128 | 98.11% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Permuter-class FP register coloring: float temp `v` and the 0.2/0.05/0.015 literal scratch |
| func_00203D30 | 0x00203D30 | 0x128 | 78.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Permuter-class regalloc / frame-size: target keeps 5 callee-saved regs (s0-s4 = arg1/arg2/ |
| func_001AD010 | 0x001AD010 | 0x12C | 83.45% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW instruction-scheduling + CW-specific redundant mask (same family as sibling func_001ACE |
| func_001BBBF0 | 0x001BBBF0 | 0x12C | 84.97% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | instruction-scheduling permutation: original CW 2.3.1 schedules the absolute-address minue |
| func_001A8840 | 0x001A8840 | 0x130 | 97.24% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | branch-delay-slot scheduling artifact: both mwcc 2.3.3 and 991202 hoist the trailing *(sho |
| func_001A9B10 | 0x001A9B10 | 0x130 | 87.30% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + scheduling permutation (NOT clean-store nop). Body/structure 100% reco |
| func_001AE6B0 | 0x001AE6B0 | 0x130 | 94.25% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | List-scheduler / register-allocation artifact (permuter territory). Body and logic fully r |
| func_001D1AE0 | 0x001D1AE0 | 0x130 | 83.62% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | List-scheduler / register-allocation artifact (permuter territory). Body and logic fully r |
| func_0016BE40 | 0x0016BE40 | 0x134 | 96.60% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body 100% correct on both builds (96.6% on 2.3.3, only 5 prologue instructions differ). Re |
| func_001AAD00 | 0x001AAD00 | 0x134 | 68.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Documented scheduler-divergence wall, reproduced exactly. All instructions (every lw/lh lo |
| func_002283B0 | 0x002283B0 | 0x138 | 99.81% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-coloring permutation: body byte-identical, only 2 arg-register diffs remain (disp |
| func_001B7700 | 0x001B7700 | 0x138 | 91.73% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | branch-sense / block-layout artifact: outer (a2+8)==2||==0 lowers to beq+bnez vs target's  |
| func_001581A0 | 0x001581A0 | 0x13C | 91.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-coloring permutation: body byte-correct, residual is the (1<<x) shift-base consta |
| func_00191580 | 0x00191580 | 0x13C | 96.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-register-coloring permutation: body and control flow byte-structure identical; target u |
| func_001469B0 | 0x001469B0 | 0x140 | 93.69% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Regalloc + scheduling permutation: control flow and all memory ops byte-structure identica |
| func_001D6C90 | 0x001D6C90 | 0x13C | 96.14% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Regalloc + scheduling permutation: all loads/stores/shifts and the wide bitfield packing a |
| func_00177F40 | 0x00177F40 | 0x140 | 89.75% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring permutation. Body and structure fully recovered. The target computes  |
| func_00181E20 | 0x00181E20 | 0x140 | 97.12% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residual artifacts at 97.125% on mwcc233 (best). (1) FP even/odd register-coloring on  |
| func_0018CA90 | 0x0018CA90 | 0x140 | 87.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP even/odd register-coloring + load-batch scheduling permutation. Body fully recovered. T |
| func_0019C6F0 | 0x0019C6F0 | 0x140 | 72.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation/coloring permutation (identical instruction sequence and operands, onl |
| func_00226D50 | 0x00226D50 | 0x140 | 99.25% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Cross-branch shared-constant delay-slot fill: target hoists a0=1 into the beqz delay slot  |
| func_001FC280 | 0x001FC280 | 0x140 | 84.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Dead-temporary register coloring: the final pointer add (record + D_00810702*0x30) and its |
| func_001ACEC0 | 0x001ACEC0 | 0x148 | 97.50% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Delay-slot-fill scheduling wall: in the case-0 arm mwcc speculatively hoists `lui at, 0x70 |
| func_00182BF0 | 0x00182BF0 | 0x148 | 69.87% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW branch-inversion/dead-b idiom dominates: each of the ~7 `return 1` exits is lowered by  |
| func_001D42E0 | 0x001D42E0 | 0x14C | 94.82% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | param-save emit order in prologue: target emits paddub s1,a1 before sq s0 and fills the fi |
| func_001FCF90 | 0x001FCF90 | 0x148 | 93.07% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | saved-register coloring permutation: q/loop-counter/count/buffer-pointer occupy a permuted |
| func_00176390 | 0x00176390 | 0x14C | 91.67% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | backend artifacts: (1) target speculates paddub a0,s2 into the 'if(e==NULL)' beqz delay sl |
| func_001582E0 | 0x001582E0 | 0x150 | 96.43% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation + scheduling near-miss (best on mwcc 2.3.3, 96.4%; 991202 only 83.7%). |
| func_001ECFB0 | 0x001ECFB0 | 0x14C | 67.36% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + scheduling near-miss (best on mwcc 991202, 84.1%; 233 only 67.4%). Bod |
| func_00120B98 | 0x00120B98 | 0x150 | 83.60% | ee-gcc 2.9-991111-01 | This is an EE-GCC function, NOT mwcc (target uses sd/ld + daddu register saves, not mwcc s |
| func_00180850 | 0x00180850 | 0x154 | 99.76% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | prologue parameter-save MOVE-ORDER permutation: target emits a1->s1 (flag) before a0->s3 ( |
| func_00158BD0 | 0x00158BD0 | 0x154 | 90.62% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | two regalloc/scheduling permutations: (1) dispatch save-sink — target keeps switch byte in |
| func_0018D7B0 | 0x0018D7B0 | 0x154 | 96.94% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW-vs-mwcc branch lowering: target keeps explicit `beq s1,1,body / nop / b epilogue` (ente |
| func_001FC3C0 | 0x001FC3C0 | 0x154 | 86.94% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring (s0/s1/s2 assignment order) + one scheduling reorder of the f |
| func_00199C50 | 0x00199C50 | 0x158 | 90.93% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Loop-counter / table-index register coloring + minor scheduling of the 2nd loop's mult set |
| func_00176180 | 0x00176180 | 0x158 | 97.24% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Single FP-load scheduling/coloring artifact: target hoists the *0x700031C8 lwc1 into the s |
| func_001DA080 | 0x001DA080 | 0x158 | 98.81% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring permutation: body and structure are byte-correct except for a |
| func_001E0380 | 0x001E0380 | 0x158 | 94.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-register coloring + scheduling permutation. After fixing frame size (sp90 is a 0x40-byt |
| func_001AA140 | 0x001AA140 | 0x15C | 93.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + instruction-scheduling permutation (NOT the clean-store nop). Body 100 |
| func_001AAA90 | 0x001AAA90 | 0x164 | 99.27% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Single-instruction residual: both mwcc builds speculatively hoist the store-block address  |
| func_001C5FB0 | 0x001C5FB0 | 0x164 | 86.57% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/control-flow byte-identical; every residual is a saved-register coloring permutation  |
| func_00127E40 | 0x00127E40 | 0x160 | 50.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Compiler-version / stack-codegen wall: target uses a 0x50 frame with 8-byte `sd` $ra/$s0 s |
| func_001D3F60 | 0x001D3F60 | 0x164 | 88.09% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring + entry callee-save order. Body/structure fully faithful (op- |
| func_00177B80 | 0x00177B80 | 0x168 | 95.07% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Argument-setup scheduling + one int-constant regalloc. Body/structure/logic fully faithful |
| func_00127230 | 0x00127230 | 0x168 | 94.56% | ee-gcc 2.9-991111-01 | ee-gcc register-allocation coloring. NOTE: this func is Sony PS2 SDK softfloat-library cod |
| func_001B30E0 | 0x001B30E0 | 0x168 | 98.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation. Body/structure/control-flow/branch-senses/FP-compare orde |
| func_0019A910 | 0x0019A910 | 0x16C | 81.29% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring + delay-slot scheduling. Body/structure fully recovered and v |
| func_001B1630 | 0x001B1630 | 0x16C | 96.87% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP instruction-scheduling / register-coloring permutation of the three independent anchor- |
| func_001BC560 | 0x001BC560 | 0x16C | 90.77% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc software-pipelines the three independent trailing global float accumulates (D_008101F |
| func_001AF8E0 | 0x001AF8E0 | 0x170 | 89.18% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling / register-coloring permutation of the trailing gp-rel pointer-tabl |
| func_001B2F70 | 0x001B2F70 | 0x170 | 98.37% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation permutation (callee-saved coloring + a0/a1 param-save order); body byt |
| func_001D5170 | 0x001D5170 | 0x170 | 78.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation permutation (s-reg coloring of arg0/dst/savedip + cached gp-ptr temp t |
| func_00184BA0 | 0x00184BA0 | 0x174 | 98.23% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation permutation (callee-saved coloring cnt/pp/obj/winner across s0-s3); bo |
| func_00198D90 | 0x00198D90 | 0x174 | 98.82% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP add.s operand coloring (+3.0f: target f1+f0 vs mwcc f0+f1) and case-0 branch-likely spe |
| func_001383C0 | 0x001383C0 | 0x178 | 98.63% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Outer mode-switch dispatch lowering: target hoists the beqz (mode 0) test ahead of the beq |
| func_0015AE20 | 0x0015AE20 | 0x178 | 99.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Switch default-branch delay slot: when state >= 2 the target branches to the tail with an  |
| func_0019A570 | 0x0019A570 | 0x178 | 84.72% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation COLORING permuter near-miss (84.72% mwcc 2.3.3). Structure+frame size  |
| func_001DFF70 | 0x001DFF70 | 0x178 | 91.97% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation COLORING + minor FP scheduling near-miss (91.97% mwcc 2.3.3). Switch l |
| func_001D9720 | 0x001D9720 | 0x178 | 57.95% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | u64 GS-register bitfield-struct reconstruction wall (57.95% mwcc 2.3.3, ~75/101 instructio |
| func_001C3BE0 | 0x001C3BE0 | 0x180 | 99.79% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring permutation: target colors the final vec3 copy temps f2/f1/f0 (descen |
| func_001B6BF0 | 0x001B6BF0 | 0x180 | 92.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Multi-artifact near-miss (93.58% mwcc991): inverse-CSE of constant 8, CW empty branch-dela |
| func_001EC270 | 0x001EC270 | 0x180 | 66.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring + instruction-scheduling near-miss (82.18% mwcc991). All addressing m |
| func_001EC470 | 0x001EC470 | 0x180 | 66.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FPU register-coloring + commutative add.s operand-order permutation. CW colors the int->fl |
| func_001EC820 | 0x001EC820 | 0x180 | 66.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FPU register-coloring + commutative add.s operand-order permutation (identical class to fu |
| func_001FABF0 | 0x001FABF0 | 0x180 | 98.75% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Parameter-save-order permutation. CW target saves arg2(scale)->s0 before the 'beqz a3' gua |
| func_00158D30 | 0x00158D30 | 0x184 | 86.86% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring on the outer 4-way dispatch (dispatch-save-sink): the target  |
| func_00164220 | 0x00164220 | 0x184 | 99.59% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring permutation at 99.588% (mwcc233). Body and all four state-mac |
| func_0019F1A0 | 0x0019F1A0 | 0x184 | 92.27% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring at 92.268% (mwcc233). Every instruction is in the EXACT targe |
| func_0021E830 | 0x0021E830 | 0x184 | 99.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | branch-likely-sense permutation: the case-1 float guard `if(arg0[0x3C] <= 160.0f)` emits a |
| func_001CD390 | 0x001CD390 | 0x188 | 97.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | branch-delay-slot scheduling permutation: the dual float-equality guard `if (arg1[0]==0.0f |
| func_00210030 | 0x00210030 | 0x188 | 87.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring permutation (target pins temps to volatile $f2/$f3 across whole funct |
| func_001DD7B0 | 0x001DD7B0 | 0x188 | 64.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Whole-function instruction scheduling + register coloring over a packed GS-bitfield read-m |
| func_0019A180 | 0x0019A180 | 0x18C | 86.83% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-register-coloring permutation: target colors v into f1 + each const into f0 (c.lt.s/c.l |
| func_0015A070 | 0x0015A070 | 0x190 | 88.90% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring permutation on the outer switch dispatch: target keeps state byte live i |
| func_00229A70 | 0x00229A70 | 0x18C | 78.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two CodeWarrior codegen artifacts: (1) arg-eval/scheduling order on the leading func_001FE |
| func_001F1DD0 | 0x001F1DD0 | 0x190 | 97.69% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling near-miss (permuter territory). Body and control flow byte- |
| func_001E0E80 | 0x001E0E80 | 0x190 | 86.12% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring near-miss (permuter territory). Body, control flow, stack layout (sp50 a |
| func_00181730 | 0x00181730 | 0x1A0 | 95.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register coloring on the (20.5+B4) temp: target colors the 5.0f constant into f1 / B4 i |
| func_00182DF0 | 0x00182DF0 | 0x1A0 | 99.18% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two artifacts at 99.18% (233): (1) mwcc hoists the lui at,0x7000 of the post-`b` 0x70003B8 |
| func_00189FE0 | 0x00189FE0 | 0x1A0 | 98.26% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two artifacts at 98.26% (233): (1) mwcc materializes the sp+0x3C store address (addiu v0,s |
| func_001AEBE0 | 0x001AEBE0 | 0x1A0 | 99.47% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation. Body + structure fully recovered and byte-identical |
| func_002117D0 | 0x002117D0 | 0x1A0 | 90.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation (FP odd-half coloring + arg-pointer lifetime split). Full  |
| func_00128640 | 0x00128640 | 0x1A4 | 99.62% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring permutation. Full logic recovered (three early-return gates on 0x7000 |
| func_0012E0B0 | 0x0012E0B0 | 0x1A4 | 99.05% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW branch-target-alignment nop: original CodeWarrior pads a trailing `nop` (in the bnez de |
| func_001747F0 | 0x001747F0 | 0x1A4 | 97.86% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two register-coloring/scheduling permutations (body byte-identical): (1) original CW defer |
| func_0017F9E0 | 0x0017F9E0 | 0x1A4 | 99.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Single speculative delay-slot fill (body byte-identical): mwcc hoists the post-if merge bl |
| func_001AF2C0 | 0x001AF2C0 | 0x1A4 | 80.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling permutation of trailing independent global stores. Insns 0-29 (zero |
| func_0021D2E0 | 0x0021D2E0 | 0x1A4 | 99.90% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP operand-coloring on a single commutative add.s. Body + structure 100% recovered and byt |
| func_00151200 | 0x00151200 | 0x1A8 | 90.98% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + branch-likely-sense permutation in the 4-iteration loop. Body + struct |
| func_001B55E0 | 0x001B55E0 | 0x1A8 | 87.40% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/instruction-scheduling permutation. Body+structure fully recovered. Tw |
| func_001D4FC0 | 0x001D4FC0 | 0x1A8 | 81.12% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation. Body+control-flow byte-correct in shape (caching D_ |
| func_0019B8C0 | 0x0019B8C0 | 0x1B4 | 91.32% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation / instruction-scheduling permutation. Body+structure fully recovered ( |
| func_00148520 | 0x00148520 | 0x1B8 | 99.27% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + instruction-scheduling permutation at 99.27% (mwcc 2.3.3). Body fully  |
| func_002079F0 | 0x002079F0 | 0x1B4 | 98.73% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + branch-sense permutation at 98.73% (mwcc 2.3.3). Body fully correct. R |
| func_0019BA80 | 0x0019BA80 | 0x1B4 | 92.83% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Callee-saved register-coloring ROTATION at 92.84% (mwcc 2.3.3). Body fully correct, frame  |
| func_001A9000 | 0x001A9000 | 0x1B8 | 94.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW-vs-mwcc branch-delay-slot fill at the two loop back-edges: the CW target leaves the bac |
| func_001EB440 | 0x001EB440 | 0x1B8 | 94.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring permutation in the SECOND of two identical spawn passes (the first pa |
| func_00188ED0 | 0x00188ED0 | 0x1BC | 93.47% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two genuine compiler artifacts downstream of correct logic: (1) one-slot scheduling of the |
| func_001FD790 | 0x001FD790 | 0x1C0 | 57.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order swap: target colors idx->$s2 / frame->$s0, mwcc colors idx->$s0  |
| func_0022E630 | 0x0022E630 | 0x1C0 | 85.01% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP odd-half register coloring + scheduling permutation. Body/structure/signature/all-globa |
| func_0015BF90 | 0x0015BF90 | 0x1C8 | 90.62% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two CW-vs-mwcc compiler artifacts (not the clean-store nop): (1) min(a,b) lowers to bc1fl  |
| func_0014D5F0 | 0x0014D5F0 | 0x1CC | 99.83% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-arg-emit-order: the single anim_clip_init float-arg pair emits mtc1 f12(5.0) before mtc |
| func_0017E510 | 0x0017E510 | 0x1CC | 99.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Arg-setup-order/scheduling: in func_00102C58(&D_700036A0,&D_700036A0,arg0+0xC0) the target |
| func_0018A6B0 | 0x0018A6B0 | 0x1CC | 87.20% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + constant-sharing + tail scheduling (991202==233, 87.2%). Body, the s1= |
| func_001A8660 | 0x001A8660 | 0x1D4 | 96.05% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc fuses the masked array-index multiply (d & 0xFF) * 4 into a single 'sll v0,a0,2' wher |
| func_00141D20 | 0x00141D20 | 0x1D8 | 90.09% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + switch-dispatch artifact (identical 90.1% on both builds, so not a cle |
| func_0013B5B0 | 0x0013B5B0 | 0x1DC | 95.21% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | regalloc + scheduling near-miss (95.2% mwcc233): param/temp register coloring (target arg1 |
| func_00153770 | 0x00153770 | 0x1DC | 87.10% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | scheduling + block-ordering near-miss (87.1% mwcc233): target schedules `subu t34-step` in |
| func_001D6930 | 0x001D6930 | 0x1D8 | 64.92% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | pervasive regalloc-coloring near-miss (64.9% mwcc233): instruction sequence matches but ta |
| func_0012DE90 | 0x0012DE90 | 0x1E0 | 98.25% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Permuter-class on a fully-recovered body. Residual 1: commutative add.s FP-coloring/operan |
| func_00199FA0 | 0x00199FA0 | 0x1DC | 88.32% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Permuter-class register coloring on a fully-recovered body. Branch/early-exit structure ma |
| func_0019AB20 | 0x0019AB20 | 0x1E0 | 93.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Permuter-class on a fully-recovered body/structure. Residual 1: saved-register coloring sw |
| func_001F18C0 | 0x001F18C0 | 0x1E0 | 98.25% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Scheduling/canonicalization permutation (not the clean-store nop, 2.3.3 cannot fix). Body, |
| func_0013D980 | 0x0013D980 | 0x1E8 | 98.16% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Prologue register-coloring / scheduling artifact. Entire body is byte-identical; the sole  |
| func_00147960 | 0x00147960 | 0x1E4 | 96.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two coupled compiler artifacts on a fully-recovered body. (1) The 16-wide table-address in |
| func_001751A0 | 0x001751A0 | 0x1E4 | 97.27% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Delay-slot-fill / instruction-scheduling artifact on a fully-recovered body. The target ho |
| func_0014C490 | 0x0014C490 | 0x1E8 | 99.92% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP even/odd register-coloring permutation (permuter-class, same class as func_001509A0 NEA |
| func_0014BC30 | 0x0014BC30 | 0x1E8 | 87.60% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP even/odd register-coloring permutation + constant-CSE difference (permuter-class). Body |
| func_001C92C0 | 0x001C92C0 | 0x1E8 | 86.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP even/odd register-coloring permutation (permuter-class, same as func_001509A0). Body, t |
| func_00198050 | 0x00198050 | 0x1EC | 88.37% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Scheduling/FP-coloring near-miss. Body + control flow fully recovered. Residual: mwcc load |
| func_00199DB0 | 0x00199DB0 | 0x1EC | 84.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Early-return guard branch-sense + FP-coloring near-miss. Logic fully recovered. Residual:  |
| func_00198AF0 | 0x00198AF0 | 0x1F0 | 94.68% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + delay-slot scheduling near-miss (identical 94.68% on both builds). Bod |
| func_001BF3C0 | 0x001BF3C0 | 0x1F0 | 87.20% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation / constant-CSE + delay-slot-fill scheduling permutation. Body and cont |
| func_001EB250 | 0x001EB250 | 0x1F0 | 64.54% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Float-constant materialization order + FP register coloring (scheduling permutation). Body |
| func_001EB600 | 0x001EB600 | 0x1F0 | 89.90% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring + float-constant scheduling in the two func_001CFB50 arg setups. Body |
| func_00205050 | 0x00205050 | 0x1F0 | 88.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residual codegen artifacts after full logic recovery (88.9% mwcc233 vs 69.6% 991202):  |
| func_00131740 | 0x00131740 | 0x1F4 | 93.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling / delay-slot-fill / branch-likely lowering permutation. Body and st |
| func_0016BC40 | 0x0016BC40 | 0x1F8 | 94.41% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body-correct near-miss at 94.4% (mwcc 2.3.3; 991202=85.9%). Full per-state switch dispatch |
| func_001D40E0 | 0x001D40E0 | 0x1F8 | 96.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body-correct near-miss at 96.5% (mwcc 2.3.3; 991202=76.2%). Full logic recovered: VIF/DMA  |
| func_001D4440 | 0x001D4440 | 0x1F8 | 96.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body-correct near-miss at 96.5% (mwcc 2.3.3; 991202=75.7%). Exact twin of func_001D40E0 (s |
| func_00137C80 | 0x00137C80 | 0x200 | 99.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-arg emit-order artifact. Body is byte-identical except ONE swapped pair: the anim_clip_ |
| func_00198240 | 0x00198240 | 0x200 | 98.40% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Branch delay-slot fill artifact. Body byte-identical except the `r == 1` dispatch branch:  |
| func_0012ADC0 | 0x0012ADC0 | 0x1FC | 84.02% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | List-scheduling + param-save-order artifact across a large global-store block. The control |
| func_0019B2C0 | 0x0019B2C0 | 0x200 | 96.64% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | idiom-13 const-store delay-slot NOP x2: target leaves a NOP in the beqz delay slot before  |
| func_0019B4C0 | 0x0019B4C0 | 0x200 | 96.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | idiom-13 const-store delay-slot NOP x2 (else-store 0x700031D0 + flags&0x80000000 guard, ta |
| func_001FC7B0 | 0x001FC7B0 | 0x200 | 61.16% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Whole-function register-allocation ORDER on a recursive text-wrap parser: structure, branc |
| func_00225AC0 | 0x00225AC0 | 0x200 | 95.54% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Scheduling near-miss. Body byte-identical after goto-shared-return-0 reshape + large-array |
| func_00198440 | 0x00198440 | 0x204 | 96.67% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-coloring + scheduling near-miss. Body byte-identical after fixing the float const (0.4f |
| func_002044F0 | 0x002044F0 | 0x204 | 92.40% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation near-miss. Logic fully recovered & body byte-identical (fixed the 64-b |
| func_00147390 | 0x00147390 | 0x210 | 95.98% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Outer mode-switch dispatch lowering: the target tests the mode byte in DESCENDING order (b |
| func_001D4750 | 0x001D4750 | 0x210 | 93.42% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residual regions, both compiler artifacts (not the clean-store nop). (1) GS/DMA packet |
| func_0013EB90 | 0x0013EB90 | 0x214 | 92.26% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling permutation (NOT clean-store delay-slot nop). Logic + struc |
| func_0015AC00 | 0x0015AC00 | 0x214 | 91.09% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling/CSE artifact (NOT clean-store nop): target folds the shared 2.0f co |
| func_00159970 | 0x00159970 | 0x214 | 77.35% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Stacked artifacts (NOT clean-store nop): (1) argument hoisting -- target computes anim=arg |
| func_001416D0 | 0x001416D0 | 0x214 | 80.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc list-scheduling artifact (NOT clean-store nop): the float-copy block interleaves lwc1 |
| func_0019A6F0 | 0x0019A6F0 | 0x214 | 93.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register coloring (s0=mode,s1=flags,s2=base,s3=arg0) and the 3-way sub-update dispatch now |
| func_001756E0 | 0x001756E0 | 0x218 | 94.34% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body and control flow fully correct (branch-likely beql on func_001760C0 reproduced). Sole |
| func_001B37D0 | 0x001B37D0 | 0x218 | 81.85% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Identical call set, arguments, and nested (7 rings x 2 signs) loop structure as the target |
| func_002034C0 | 0x002034C0 | 0x220 | 96.10% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling permutation (body/logic 100% correct): (1) the two func_00108DB0 ca |
| func_00178080 | 0x00178080 | 0x220 | 89.59% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring permutation (body/logic/structure 100% correct): the target keeps the th |
| func_001FDB80 | 0x001FDB80 | 0x224 | 59.74% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation + inverse-CSE near-miss. Logic fully recovered and correct. Residuals  |
| func_00228C90 | 0x00228C90 | 0x220 | 86.40% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Pure register-allocation permutation near-miss. Logic fully recovered and correct (all ins |
| func_001EC5F0 | 0x001EC5F0 | 0x22C | 95.32% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant scheduling permutation isolated to the MIDDLE (2nd) of three identical passes: |
| func_00153540 | 0x00153540 | 0x22C | 99.58% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residuals at 99.58%: (1) commutative add.s operand-coloring on the three pose+=velocit |
| func_001B62C0 | 0x001B62C0 | 0x228 | 88.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CW-vs-mwcc branch lowering of the 4-way octant selector. The mula.s/madd.s MAC pair for dx |
| func_001FD950 | 0x001FD950 | 0x22C | 85.83% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation + instruction-scheduling + branch-likely-sense permutation. Body fully |
| func_001EB020 | 0x001EB020 | 0x230 | 95.53% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring + constant-emit-order permutation in the 3rd of 3 identical passes (f |
| func_002280F0 | 0x002280F0 | 0x22C | 85.14% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation + branch-likely-sense + arg-emit-order permutation across the state ma |
| func_001B18F0 | 0x001B18F0 | 0x238 | 99.23% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation: mwcc 2.3.3 (and 991202) map the three pointer param |
| func_00157360 | 0x00157360 | 0x248 | 91.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring + arg-eval-order + FP odd/even-half coloring scheduling artifacts in a 0 |
| func_001814E0 | 0x001814E0 | 0x248 | 97.35% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 97.35% mwcc233 near-miss (logic fully recovered). Residual = arg-setup register order for  |
| func_001D1C50 | 0x001D1C50 | 0x248 | 95.96% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 95.96% mwcc233 near-miss (logic fully recovered). Residual = register coloring in the D_00 |
| func_00177CF0 | 0x00177CF0 | 0x250 | 99.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring + arg-move-order permutation (f0/f1 swap on the local-vector setup an |
| func_00199770 | 0x00199770 | 0x250 | 97.39% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FPU register-coloring permutation in the distance sqrt inputs and the two-coordinate scale |
| func_00120CE8 | 0x00120CE8 | 0x254 | 75.62% | ee-gcc 2.9-991111-01 | ee-gcc register-allocation-ORDER permutation across the whole body (target holds -1 in s5, |
| func_001CC8A0 | 0x001CC8A0 | 0x254 | 77.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc-build ABI/register-model mismatch (NOT clean-store nop, NOT fixable by C reshaping).  |
| func_001FB9F0 | 0x001FB9F0 | 0x254 | 82.00% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/instruction-scheduling permutation across a large (0x254-byte) 6-way s |
| func_00146F80 | 0x00146F80 | 0x260 | 87.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling / boolean-materialization permutation, same family as the documente |
| func_0014A730 | 0x0014A730 | 0x264 | 95.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (3-state switch/fallthrough dispatcher on arg0+6, anim_clip |
| func_001A9480 | 0x001A9480 | 0x268 | 85.47% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered: dispatch on arg1+3==0xB (special-item path scans a 12-entr |
| func_001BA580 | 0x001BA580 | 0x268 | 91.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered: a 21-level cascading != chain classifying the sound-effect |
| func_00185E30 | 0x00185E30 | 0x26C | 96.10% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Pure FP odd/even register coloring (fv0/fv0f/fv1/fv1f family) on the sum-of-squares distan |
| func_001A8970 | 0x001A8970 | 0x26C | 98.23% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Pure FP saved-register coloring: target keeps the f21 running total (proximity guard's hal |
| func_0014A0D0 | 0x0014A0D0 | 0x274 | 93.66% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two stacked genuine compiler artifacts (body/structure fully recovered, byte-verified equi |
| func_0014E050 | 0x0014E050 | 0x270 | 97.24% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two stacked genuine compiler artifacts (body/structure fully recovered): (1) the documente |
| func_00178910 | 0x00178910 | 0x27C | 92.67% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring permutation of the loop-pointer set (flags/tsin/tpos/i shifted by one re |
| func_00158590 | 0x00158590 | 0x278 | 86.95% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-half coloring in the three vec3-add chains plus argument-emission-order for fu |
| func_001BB860 | 0x001BB860 | 0x274 | 57.68% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Deep register-allocation + branch-shape permutation across the nested state-dispatch (targ |
| func_001C1A80 | 0x001C1A80 | 0x27C | 97.83% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two fused compiler artifacts, both confirmed by full instruction-level diff (every branch/ |
| func_00179910 | 0x00179910 | 0x280 | 89.06% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation / delay-slot-fill order permutation. Body and control flow are 100% co |
| func_001854E0 | 0x001854E0 | 0x280 | 88.66% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring on the sum-of-squares (adda.s/madd.s) idiom -- identical wall class a |
| func_001D2300 | 0x001D2300 | 0x280 | 80.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation (extra callee-saved register / one different cached  |
| func_0018CBD0 | 0x0018CBD0 | 0x284 | 91.78% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc saved-float-register-coloring wall: target allocates $f21 for the speed->dist->ang ca |
| func_001C7EB0 | 0x001C7EB0 | 0x28C | 89.07% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Same class of mwcc saved-register-coloring wall as func_0018CBD0: target assigns s1=tail/s |
| func_0020E460 | 0x0020E460 | 0x28C | 93.18% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Switch-dispatch register-coloring / delay-slot-hoist / CSE-sharing scheduling artifact: ta |
| func_001999C0 | 0x001999C0 | 0x290 | 83.20% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring permutation ($s0/$s1 hold scratch/result swapped vs. target) plus per-br |
| func_001D66A0 | 0x001D66A0 | 0x28C | 67.06% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FPU-MAC wall (confirmed, dead class): the target computes the four rotation-seed floats us |
| func_001E2560 | 0x001E2560 | 0x294 | 98.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Genuine idiom-13 clean-constant-store nop wall: `*(arg0+4)=3` early-return stores material |
| func_002036E0 | 0x002036E0 | 0x294 | 93.33% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two backend artifacts, both confirmed non-C-reproducible: (1) $at-vs-GPR register naming o |
| func_00187EE0 | 0x00187EE0 | 0x298 | 90.00% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | idiom-13 delay-slot wall on address-lui speculation, 3 residual branches (case 0x5A/0x5B/0 |
| func_001DDB70 | 0x001DDB70 | 0x29C | 63.66% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | GS-packet struct-field/scheduling divergence: deep-offset field writes and a &D_0026E810 a |
| func_0015BCF0 | 0x0015BCF0 | 0x2A0 | 97.34% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc canonicalization of a 2-case dense switch (case 3/case 4 shared body): mwcc always te |
| func_001C7C00 | 0x001C7C00 | 0x2A4 | 93.85% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | see wall field above |
| func_00161020 | 0x00161020 | 0x2A8 | 81.50% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 81.5% (mwcc233; 991202 69.0%). register-allocation-ORDER: dispatch structure, all |
| func_00126F88 | 0x00126F88 | 0x2A8 | 59.59% | ee-gcc 2.9-991111-01 | This function is Sony PS2 SDK softfloat-library code (module tag 'lowmem' in FUNCTIONS.csv |
| func_001BD9F0 | 0x001BD9F0 | 0x2AC | 78.07% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS: 6-way state-machine switch (arg0+4) with nested 5-way switch on arg0+5, fully re |
| func_00197490 | 0x00197490 | 0x2AC | 91.04% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS: 3-arg outer-switch on arg1+0x230 (an animation/anim-event id) dispatching to a s |
| func_0017B660 | 0x0017B660 | 0x2A8 | 82.85% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS: player-character bone-matrix-transition writer (documented in docs/FINDINGS.md ~ |
| func_001F5F60 | 0x001F5F60 | 0x2AC | 93.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling permutation only. Body and control flow are byte-for-byte s |
| func_00153290 | 0x00153290 | 0x2B0 | 93.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling permutation only. Body and control flow structurally identi |
| func_00185760 | 0x00185760 | 0x2B0 | 96.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation confined to the opening prologue block. Structurally ident |
| func_00150F40 | 0x00150F40 | 0x2B4 | 93.01% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-coloring permutation across the table-lookup block (see wall notes) |
| func_0017E250 | 0x0017E250 | 0x2B8 | 99.42% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Arg-setup-order/scheduling wall on func_00102C58(&D_700036A0,&D_700036A0,arg0+0xC0): targe |
| func_001FDDB0 | 0x001FDDB0 | 0x2B8 | 94.57% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Multiple scheduling/branch-shape artifacts on an otherwise byte-faithful 96-instruction bo |
| func_0012F6C0 | 0x0012F6C0 | 0x2BC | 99.63% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Sole residual is a single instruction: idiom-13 clean-constant-store delay-slot wall. Both |
| func_0014A350 | 0x0014A350 | 0x2C4 | 97.06% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Idiom-13b dead-const re-materialization: two dispatch-chain constants (the case-2/3 `addiu |
| func_001F28D0 | 0x001F28D0 | 0x2C4 | 82.98% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation across the two 8-iteration loops (same class as sibling fu |
| func_0013BF20 | 0x0013BF20 | 0x2CC | 94.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | clean-store delay-slot-nop x2 + independent-instruction scheduling swap x2 (scheduler-inte |
| func_00179D20 | 0x00179D20 | 0x2CC | 99.82% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (per-bone anim-clip pose seed loop over D_00275B40[]; scala |
| func_00148130 | 0x00148130 | 0x2D0 | 95.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered: 4-state animation blend-transition state machine (state fi |
| func_00159B90 | 0x00159B90 | 0x2D4 | 85.72% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered: outer 4-state actor dispatch (p[4], cases 0-3) with case1  |
| func_00198650 | 0x00198650 | 0x2D4 | 94.17% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Idiom-13 conditional-branch delay-slot NOP (3 instances, mwcc fills with next compare's sp |
| func_001DB9D0 | 0x001DB9D0 | 0x2D4 | 73.05% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FPU-MAC wall (genuine, confirmed against sibling func_001D66A0 which documents the identic |
| func_001EBF10 | 0x001EBF10 | 0x2D4 | 56.72% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-constant emit-order / instruction-scheduling permutation, same wall class already docum |
| func_001F3340 | 0x001F3340 | 0x2D8 | 80.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Switch-dispatch head (15 case labels, non-monotonic target test order) fully recovered by  |
| func_0022B420 | 0x0022B420 | 0x2D8 | 87.95% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/logic fully recovered (state dispatch 0/1/2, screen-position compute via float_to_int |
| func_0012E560 | 0x0012E560 | 0x2DC | 91.53% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/logic fully recovered (spawn-counter bump, owner-record init via two func_00102948 co |
| func_001B3F10 | 0x001B3F10 | 0x2DC | 97.97% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 97.97% (mwcc233; 991202 82.34%). Logic and structure fully recovered and body-ide |
| func_001F2BA0 | 0x001F2BA0 | 0x2E4 | 94.43% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation permutation across the case-1 scan loop (s0-s4 relabeled end-to-end vs |
| func_0021B550 | 0x0021B550 | 0x2E4 | 98.43% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residuals prevent 100.0 on mwcc233: (1) case-2 'st2 != 1 / st2 != 0' guard -- target k |
| func_00152040 | 0x00152040 | 0x2E8 | 98.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation SAVE-ORDER permutation: with two pointer params both live across calls |
| func_00225D20 | 0x00225D20 | 0x2E8 | 99.30% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | idiom-13 refined wall (proven ungeneralizable): the sole residual is a conditional-branch  |
| func_001370C0 | 0x001370C0 | 0x2E8 | 96.88% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Paired float-register scheduling/coloring artifact (FPU-coloring class): after the func_00 |
| func_0019E640 | 0x0019E640 | 0x2EC | 93.64% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring/regalloc-order residual only: instruction count matches (191==191 mwcc23 |
| func_001E8B90 | 0x001E8B90 | 0x2EC | 71.53% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring/scheduling residual in the FP compare chain (198==198 instructions, stru |
| func_001C64F0 | 0x001C64F0 | 0x2F0 | 92.39% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Best build is mwcc991202 (93.16%) vs mwcc233 (92.39%). Body/control-flow fully recovered ( |
| func_0021AE90 | 0x0021AE90 | 0x2F0 | 91.60% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Best build is mwcc233 (91.60%) vs mwcc991202 (87.39%). Full logic recovered and confirmed  |
| func_001E9280 | 0x001E9280 | 0x2F4 | 84.76% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Best build is mwcc233 (84.76%) vs mwcc991202 (75.04%). Full logic recovered: a GS/DMA disp |
| func_001C3DB0 | 0x001C3DB0 | 0x2FC | 93.46% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | fp-register-coloring + list-scheduler-interleave: body/structure fully recovered (cross-pr |
| func_001437E0 | 0x001437E0 | 0x304 | 96.11% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | dead-instruction-padding + fp-register-coloring: body/structure fully recovered (2-state a |
| func_00198F10 | 0x00198F10 | 0x308 | 91.95% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | dead-instruction-padding + fp-register-coloring + call-arg-schedule: body/structure fully  |
| func_001BFFD0 | 0x001BFFD0 | 0x308 | 97.24% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (switch on state byte at e+4, 4 cases dispatched by mwcc in |
| func_001A7870 | 0x001A7870 | 0x30C | 93.97% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered: O(N^2) all-pairs entity check over D_00275B90[D_00275B98]  |
| func_001DD2F0 | 0x001DD2F0 | 0x30C | 69.53% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered: two near-identical GS/DMA quad-strip particle builders. Ea |
| func_00210F30 | 0x00210F30 | 0x30C | 84.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/instruction-scheduling permutation, two spots: (1) the D_700038A0/B0/C |
| func_00138900 | 0x00138900 | 0x314 | 99.54% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two genuine residual instructions (anim_clip_init(arg0,4,0.0f,0.0f) call in state 0): targ |
| func_0014A9A0 | 0x0014A9A0 | 0x318 | 99.92% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residual instructions: the anim_clip_init(arg0,0x2B,0.0f,0.0f) call at the end of stat |
| func_001C5930 | 0x001C5930 | 0x318 | 68.64% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Large (0x318-byte) multi-state HUD-overlay handler (area-title-card driver, per docs/FINDI |
| func_00201C50 | 0x00201C50 | 0x318 | 97.25% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | see summary above |
| func_001BA1F0 | 0x001BA1F0 | 0x31C | 71.33% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Script-interpreter pump loop (opcode dispatch via ftab_0024D880). Body/control-flow fully  |
| func_00219550 | 0x00219550 | 0x31C | 83.09% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Item armed-handler pump. Body/logic fully recovered against the documented func_0015AE20-f |
| func_001BA8E0 | 0x001BA8E0 | 0x320 | 72.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NPC-spawner type dispatch (sparse compare-chain, no jump table). Body/mapping table fully  |
| func_00152330 | 0x00152330 | 0x324 | 97.96% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully correct (state-machine dispatch, all anim_clip_init calls matched via |
| func_00197D20 | 0x00197D20 | 0x324 | 89.28% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered including two fixed callee-signature bugs (func_001D2610 is |
| func_001F1AA0 | 0x001F1AA0 | 0x328 | 76.06% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Full logic recovered: the FPU-MAC dot-product (dx*dx+dy*dy+dz*dz) auto-fuses to adda.s/mad |
| func_00220D30 | 0x00220D30 | 0x328 | 85.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling permutation wall on a 3-state (0/1/2) switch dispatcher wit |
| func_0019C830 | 0x0019C830 | 0x32C | 85.20% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring/scheduling permutation. Instruction count is EXACTLY equal (219==219) be |
| func_00154120 | 0x00154120 | 0x340 | 96.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two small residual clusters after full logic/structure recovery and the gp-rel array-over- |
| func_001E10A0 | 0x001E10A0 | 0x340 | 60.43% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Cross-compiler FP list-scheduling divergence: mwcc233/991202's -O4,p scheduler hoists the  |
| func_00171320 | 0x00171320 | 0x344 | 83.99% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation-ORDER (entity-ptr vs picked-flag colored s1/s0 opposite of target) + m |
| func_001C1F50 | 0x001C1F50 | 0x344 | 72.13% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure/constants/call-arg-order fully recovered: two independent linear dispatch c |
| func_0014C680 | 0x0014C680 | 0x348 | 98.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (readable plain C matching the func_0014C9D0 sibling idiom  |
| func_001ED100 | 0x001ED100 | 0x348 | 94.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered -- exact template match with proven sibling func_001ED450 ( |
| func_00153B50 | 0x00153B50 | 0x34C | 99.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | f13-before-f12 mtc1 argument-order swap in the case-1 anim_clip_init(self,0x33,10.0f,0.0f) |
| func_0014B7B0 | 0x0014B7B0 | 0x358 | 96.22% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling artifacts inside the spad (0x700038xx) transform-block writ |
| func_001B0460 | 0x001B0460 | 0x358 | 98.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two isolated residuals after extensive iteration (ternary->if/else restructure fixed the v |
| func_0022FCA0 | 0x0022FCA0 | 0x358 | 88.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring wall (target keeps the heading-error scratch value in caller-saved fv |
| func_00208750 | 0x00208750 | 0x35C | 72.00% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Stack-frame-layout / base-pointer-sharing wall: target hoists &D_00265160 into ONE registe |
| func_00131B10 | 0x00131B10 | 0x368 | 95.14% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc-vs-CW branch-lowering / dead-trailing-block wall: target's beql-dispatched case-0 lea |
| func_00173E60 | 0x00173E60 | 0x368 | 99.95% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Sole residual is a genuine $at-vs-GPR register-coloring artifact: the `if((u8)(arg0+0x23F) |
| func_001ECB00 | 0x001ECB00 | 0x368 | 77.19% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-register-coloring / GPR-scheduling noise, not a missing/wrong operation: (1) the GS-alp |
| func_001368D0 | 0x001368D0 | 0x36C | 89.97% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residual artifacts, both scheduling/coloring not logic: (1) the first early-return gua |
| func_00162A40 | 0x00162A40 | 0x36C | 94.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Compound scheduler/register-coloring residual (three independent sites): (1) dispatch-chai |
| func_0019DF10 | 0x0019DF10 | 0x370 | 91.86% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring/scheduling permutation, same class as siblings func_0019C830 (85.2%) and |
| func_001AC480 | 0x001AC480 | 0x370 | 97.33% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Single delay-slot/scheduling residual around the D_00275BDC branch in state 0 (one nop/lui |
| func_001D7C30 | 0x001D7C30 | 0x370 | 86.53% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring (fv0/fv1/ft0 assignment permutation) plus a mwcc-vs-CW branch-lowerin |
| func_00194DB0 | 0x00194DB0 | 0x374 | 86.15% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Instruction-scheduling/materialization-order permutation. Body and structure are fully rec |
| func_001E13E0 | 0x001E13E0 | 0x37C | 81.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/frame-spill permutation. Both builds fully recover the logic (channel- |
| func_00171B00 | 0x00171B00 | 0x388 | 92.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation (same documented class as sibling func_00171320.c, w |
| func_0017AF70 | 0x0017AF70 | 0x388 | 87.23% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | see summary |
| func_0021FB40 | 0x0021FB40 | 0x388 | 91.35% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Switch-dispatch branch-scheduling artifact: frame size (0x20), single callee-saved reg (s0 |
| func_001E1AD0 | 0x001E1AD0 | 0x38C | 53.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-pressure/spill wall in a large (908-byte) 32x32-grid generator, same class as its |
| func_00193EB0 | 0x00193EB0 | 0x390 | 87.26% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | see above |
| func_001B07C0 | 0x001B07C0 | 0x390 | 99.04% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | D_00275BE0 is addressed absolute (lui+lbu) under -sdatathreshold 0 here, but the target ad |
| func_001FD0E0 | 0x001FD0E0 | 0x38C | 91.42% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring / branch-likely scheduling on a large (908-byte) 3-nested-loop state mac |
| func_00133640 | 0x00133640 | 0x394 | 91.45% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling permutation on the parallel struct-copy burst that seeds th |
| func_00146110 | 0x00146110 | 0x394 | 96.48% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Same class as the sibling func_0014B7B0 (already-parked NEARMISS): register-allocation/sch |
| func_0016A8B0 | 0x0016A8B0 | 0x394 | 94.02% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | CFG-shape-dependent instruction-scheduling artifact: in each of the 4 switch-case's true-b |
| func_001E9E60 | 0x001E9E60 | 0x3A4 | 79.75% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Genuine FPU-MAC pipeline (mula.s/madd.s) computing a per-axis lerp result=base+(target-bas |
| func_0016EBA0 | 0x0016EBA0 | 0x3AC | 93.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-ORDER wall: target loads the state byte directly into a0 (reusing arg0 |
| func_0021D800 | 0x0021D800 | 0x3B0 | 97.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling wall: mwcc233 re-materializes 'paddub a0,s0,zero' before th |
| func_00158810 | 0x00158810 | 0x3B4 | 76.12% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-ORDER wall: target keeps the arg0+0x1F0 pointer (a1) live in a caller- |
| func_001612D0 | 0x001612D0 | 0x3BC | 89.22% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | State-machine dispatch (switch on +6) resolves body/structure identically to target, but t |
| func_0016CD70 | 0x0016CD70 | 0x3B4 | 99.19% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 99.19% on mwcc233, logic and structure fully recovered (matrix/quat blend setup via func_0 |
| func_0019E280 | 0x0019E280 | 0x3B8 | 99.43% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 99.43% on mwcc233. Full logic recovered including two bugs found and fixed versus the init |
| func_0019DB50 | 0x0019DB50 | 0x3C0 | 87.77% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring/scheduling permutation, same class as sibling family func_0019DF10 (91.8 |
| func_001D2960 | 0x001D2960 | 0x3C0 | 97.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two tiny residual scheduling clusters remain out of ~240 instructions: (1) the FIRST func_ |
| func_00167C80 | 0x00167C80 | 0x3C4 | 95.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | State-machine dispatch (switch on arg0[6], ascending case labels 0-3 with nested sub-switc |
| func_00175CF0 | 0x00175CF0 | 0x3C4 | 99.15% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Clean-store/delay-slot-fill duplication: at the 'v1==0x35' guard bne, mwcc233 (and 991202  |
| func_001DF600 | 0x001DF600 | 0x3C4 | 69.20% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Deep register-allocation / stack-layout divergence in a large (964-byte, 0x10F0-frame) fun |
| func_001F1180 | 0x001F1180 | 0x3C4 | 81.62% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Deep register-allocation / block-layout divergence in the state-machine dispatch + GS-pack |
| func_0021AAC0 | 0x0021AAC0 | 0x3C4 | 93.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Saved-register allocation ORDER permutation: mwcc233 colors arg0->0x24 field and `e`(=arg0 |
| func_0017ABA0 | 0x0017ABA0 | 0x3CC | 97.40% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Residual is confined to the st2==0x31/0x34 pitch-band-scale sub-block: (1) a genuine mwcc  |
| func_001DFA40 | 0x001DFA40 | 0x3CC | 71.44% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Whole-function saved-register allocation ORDER permutation on both GP (s0-s7) and FP (f20- |
| func_00211400 | 0x00211400 | 0x3D0 | 99.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring/scheduling: at each of the 5 dispatch sites, mwcc233 loads the 3 seed |
| func_001E4610 | 0x001E4610 | 0x3D4 | 88.31% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 88.31% (233) / 82.19% (991202). Body/logic fully recovered: 4-state dispatch (0=i |
| func_00134500 | 0x00134500 | 0x3D8 | 96.82% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 96.82% (233) / 90.79% (991202) -- essentially body-complete. Two residual mwcc sc |
| func_0018C0D0 | 0x0018C0D0 | 0x3D8 | 82.52% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 82.52% (233) / 76.11% (991202). Body/logic fully recovered: two mula.s/madd.s squ |
| func_0019D770 | 0x0019D770 | 0x3D8 | 86.58% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-ORDER wall (residual, not clean-store nop). Body/control-flow/addressi |
| func_0019CF50 | 0x0019CF50 | 0x3E0 | 86.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Byte-for-byte the same function as func_0019D770 (near-identical twin, only 2 structural d |
| func_001F9CF0 | 0x001F9CF0 | 0x3DC | 86.94% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-ORDER wall (residual). Same instruction COUNT as target (256=256), fra |
| func_0017F5F0 | 0x0017F5F0 | 0x3E8 | 99.75% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP odd/even register-coloring on two timer-expiry float add/sub ops (target picks a differ |
| func_0022E7F0 | 0x0022E7F0 | 0x3E8 | 80.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc233 packs the three stack-scratch locals by SIZE (largest array always lowest offset)  |
| func_00175900 | 0x00175900 | 0x3EC | 88.90% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-permutation wall already characterized on sibling func_001756E0 (ident |
| func_002267A0 | 0x002267A0 | 0x3EC | 62.50% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/logic fully recovered (verified instruction-by-instruction against the raw .s: the ou |
| func_0019CB60 | 0x0019CB60 | 0x3F0 | 81.49% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Logic fully recovered from raw disasm: the two float-compare flag setups (D_70003190<=D_70 |
| func_001FE070 | 0x001FE070 | 0x3F0 | 85.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Logic fully recovered and cross-checked against a sibling caller (func_001FD0E0.c, which c |
| func_001BE6C0 | 0x001BE6C0 | 0x3F4 | 95.29% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-ORDER permutation (idiom exhaustion reached): after applying the 64-bi |
| func_001E8E80 | 0x001E8E80 | 0x3F4 | 87.05% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/addressing-derivation permutation (idiom exhaustion reached). The mula |
| func_0013C4C0 | 0x0013C4C0 | 0x3F8 | 98.98% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 98.98% -- extremely close. Fully recovered logic (fixed a spurious extra func_001B39F0 cal |
| func_001C02E0 | 0x001C02E0 | 0x3F8 | 94.51% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Cluster of genuine mwcc-vs-CW artifacts, logic/structure fully recovered: (1) CW emits an  |
| func_0016A4B0 | 0x0016A4B0 | 0x3FC | 94.05% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 94.05% mwcc233 (991202 83.79%). Fully recovered state-machine switch (all 7 state |
| func_001B6FA0 | 0x001B6FA0 | 0x3FC | 88.19% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 88.19% mwcc233 (991202 84.12%). Fully recovered 5-state anim blend dispatcher: al |
| func_0014CDD0 | 0x0014CDD0 | 0x408 | 94.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 94.65% (233; 991202=84.36%). Body/logic and all data fully recovered (verified against sib |
| func_0018D910 | 0x0018D910 | 0x408 | 81.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 81.89% (233; 991202=78.47%). Logic and every branch/call/global fully recovered (a large F |
| func_001399F0 | 0x001399F0 | 0x410 | 95.73% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | compiler artifact (register coloring / scheduling) |
| func_0014DC30 | 0x0014DC30 | 0x414 | 91.28% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (all 4 states decoded and structurally landed: idiom-20 bra |
| func_00185A10 | 0x00185A10 | 0x41C | 90.20% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (both the fast-path locked-target re-check and the full ent |
| func_00177030 | 0x00177030 | 0x430 | 92.32% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring + list-scheduling residual: at the switch-dispatch delay slots (case  |
| func_001F6210 | 0x001F6210 | 0x424 | 80.82% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation / callee-save-count wall: the target's per-record body fits in 5 saved |
| func_001DF180 | 0x001DF180 | 0x41C | 57.19% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation / stack-frame-size wall in a very large function (10 saved GPR incl. $ |
| func_00132490 | 0x00132490 | 0x43C | 81.40% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure/semantics fully recovered (heading-integration table dispatch: decays arg1+ |
| func_00132FB0 | 0x00132FB0 | 0x434 | 88.62% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure/semantics fully recovered (same family as func_001416D0/func_0012F6C0: seed |
| func_0019D330 | 0x0019D330 | 0x440 | 84.53% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure/semantics fully recovered (byte-identical twin of func_0019CF50/func_0019D7 |
| func_0019E930 | 0x0019E930 | 0x444 | 90.37% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation coloring (a2/a1/a0/v0/v1 consistently swapped vs target) plus a stack- |
| func_00169250 | 0x00169250 | 0x450 | 85.12% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FPU register-coloring/scheduling wall in the two func_0011E748 (dx*dx+dz*dz distance) MAC  |
| func_001ACA20 | 0x001ACA20 | 0x450 | 93.67% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring wall: the target keeps slot in a1 and the precomputed &slot[9] pointer i |
| func_001F5040 | 0x001F5040 | 0x450 | 80.45% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring wall: mwcc233 assigns arg0 to s2/step to s0/frame to s1 while the target |
| func_001486E0 | 0x001486E0 | 0x458 | 92.07% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Full 4-state logic/structure recovered (clip-init dispatch, RNG-seeded timers, quaternion- |
| func_0014D7C0 | 0x0014D7C0 | 0x46C | 91.23% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Full logic/structure recovered; notably decoded that the shipped binary reads a stale FPU  |
| func_001A06A0 | 0x001A06A0 | 0x46C | 86.42% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Full AABB-narrowing scan + hitbox-type dispatch logic recovered (types 0x1000/0x2000/0x400 |
| func_00214570 | 0x00214570 | 0x478 | 95.61% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation / branch-likely-choice permutation only. Logic, globals, and control f |
| func_001373B0 | 0x001373B0 | 0x47C | 93.64% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation / branch-likely-choice permutation only (same class as func_00214570). |
| func_001A4030 | 0x001A4030 | 0x480 | 81.64% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Fully recovered logic/structure (convex n-gon segment test, per-axis interval reject, edge |
| func_00171670 | 0x00171670 | 0x484 | 91.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Fully recovered logic/structure (entity fire-state-machine, sibling of matched func_00171B |
| func_0013C8C0 | 0x0013C8C0 | 0x488 | 93.30% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Fully recovered logic/structure using sibling func_0013C4C0 (98.98% NEARMISS) as the steer |
| func_0013EDB0 | 0x0013EDB0 | 0x488 | 94.96% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | residual scheduling artifact (see above) |
| func_00144C20 | 0x00144C20 | 0x488 | 93.27% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (enemy engage/disengage state machine, cases 0/1/2/3 on sta |
| func_001DE920 | 0x001DE920 | 0x488 | 69.43% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Full VU0/GS particle (exhaust-trail) packet-builder logic recovered (2 batches x 16 groups |
| func_0015A2C0 | 0x0015A2C0 | 0x48C | 96.63% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring / instruction-scheduling residual only (same documented family as src/fu |
| func_00135870 | 0x00135870 | 0x490 | 97.11% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Saved-reg allocation-ORDER permutation: target copies arg1(p1)->s1 THEN arg0(e)->s2 (dispa |
| func_001723D0 | 0x001723D0 | 0x48C | 86.77% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register/scheduling-artifact noise in the twice-duplicated animdispatch+copy_qw4 tail bloc |
| func_0018AF50 | 0x0018AF50 | 0x48C | 93.18% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | mwcc233 materializes small dispatch-chain constants (the case-3 literal 3) into a differen |
| func_002217C0 | 0x002217C0 | 0x4B0 | 93.75% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-permutation wall in the switch dispatch chain: body/structure/frame are byte-iden |
| func_00197870 | 0x00197870 | 0x4A4 | 93.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (two-exit float-clamp shape via the bc1t-jump-into-body idi |
| func_002082B0 | 0x002082B0 | 0x49C | 90.79% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure/frame fully recovered (all 9 saved regs match after applying the D_00275670 |
| func_0013E6D0 | 0x0013E6D0 | 0x4B4 | 99.97% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order artifact (2 instructions of 301): the hit_count>=3 sign-extend c |
| func_0018A1F0 | 0x0018A1F0 | 0x4B8 | 99.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residual clusters: (1) copy_qw4 arg register swap (v0/v1 evaluation-order regalloc noi |
| func_001C50B0 | 0x001C50B0 | 0x4BC | 88.24% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Three residual clusters at 88.24% (both compilers agree on cluster 1, ruling out a build-v |
| func_0018CE60 | 0x0018CE60 | 0x4C4 | 94.60% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/list-scheduling permutation. mwcc233's scheduler defers the 'lw a2, 0x |
| func_0013CD50 | 0x0013CD50 | 0x4C8 | 88.05% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation. Both mwcc builds place the exact same four live val |
| func_0020A7A0 | 0x0020A7A0 | 0x4C8 | 94.67% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation. Target keeps the per-slot pointer in s5 and the loo |
| func_001E55F0 | 0x001E55F0 | 0x4CC | 93.77% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation: target keeps the actor+0x1F0 pointer in $s2 and the |
| func_001C1030 | 0x001C1030 | 0x4D0 | 95.24% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation (arg0 copy colored to $s0 vs target's $s1) plus one  |
| func_001348E0 | 0x001348E0 | 0x4D8 | 95.51% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | See detailed wall description above (dead-block artifact + FPU register-pairing permutatio |
| func_001305B0 | 0x001305B0 | 0x4F8 | 99.19% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation/scheduling permutation on 4 stack-temp float reloads (target always re |
| func_00131F90 | 0x00131F90 | 0x500 | 97.93% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order artifact: target always keeps the local angular-error bucket val |
| func_001CC3B0 | 0x001CC3B0 | 0x4E8 | 62.16% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | SAME wall as sibling func_001CC8A0 (already registered in docs/NEARMISS.md): CW 2.3.1.01 A |
| func_001B8AB0 | 0x001B8AB0 | 0x504 | 84.38% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Prologue register-allocation permutation (which register holds the switch discriminant/arg |
| func_001C1570 | 0x001C1570 | 0x504 | 71.27% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Broad register-allocation/scheduling churn in the LCG loop and trig/matrix chain (large re |
| func_001424C0 | 0x001424C0 | 0x510 | 96.18% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Prologue register-allocation permutation (switch-discriminant register: target reuses $a2, |
| func_0021F330 | 0x0021F330 | 0x520 | 99.95% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring: at the arg0+0x224 and arg0+0x22C `!= 0.0f` zero-compares, target col |
| func_0013F240 | 0x0013F240 | 0x528 | 93.85% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring at the D_700038A4+=10.0f add (target: add.s f0,f1,f0 field-first; mwc |
| func_00201720 | 0x00201720 | 0x52C | 98.79% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Single root-cause delay-slot-fill artifact: target leaves the unhandled-switch-value branc |
| func_002134C0 | 0x002134C0 | 0x534 | 99.86% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation: body/schedule are byte-identical (same instruction  |
| func_001634A0 | 0x001634A0 | 0x53C | 95.84% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residual classes: (1) FP-register-coloring in the velocity-integration expr (self+0x2E |
| func_00171E90 | 0x00171E90 | 0x53C | 95.56% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Same documented saved-register-allocation-order class as sibling func_00171B00.c (already  |
| func_00143AF0 | 0x00143AF0 | 0x544 | 91.94% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order artifact: mwcc assigns arg1->s0 / arg0->s1 (opposite of the natu |
| func_00187780 | 0x00187780 | 0x53C | 78.62% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two independent non-idiom-fixable classes. (1) The stack frame carries a dead 64-bit magic |
| func_00199220 | 0x00199220 | 0x544 | 87.89% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP+GPR register-coloring permutation (documented wall class) |
| func_001D0720 | 0x001D0720 | 0x548 | 90.72% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two-phase particle-alpha/color-blend state machine (states 0-3 twice, on fields s0+0x70 an |
| func_00214020 | 0x00214020 | 0x548 | 98.62% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | UI/screen-state manager: a 5-state (0-4) machine gating sound-effect selection (func_00213 |
| func_00149B50 | 0x00149B50 | 0x574 | 94.71% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Explosion/effect-trigger state machine (5 states on arg0+6) driving anim_clip_init transit |
| func_00224600 | 0x00224600 | 0x574 | 96.93% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring (4x c.eq.s operand-order swaps on reused ==0.0f/!=0.0f tests) + mwcc  |
| func_002236F0 | 0x002236F0 | 0x57C | 99.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP register-coloring: 3 residual instructions (two !=0.0f compares reloaded into swapped f |
| func_00209860 | 0x00209860 | 0x584 | 92.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order/liveness-coloring permutation on the callee-saved set (target us |
| func_001C87C0 | 0x001C87C0 | 0x590 | 98.40% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring/scheduling permutation across the three near-identical (rotation/transla |
| func_0016BF80 | 0x0016BF80 | 0x5A0 | 93.77% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Persistent a0/a1 register-role swap for the state byte vs the `arg0` pointer across nearly |
| func_00139E00 | 0x00139E00 | 0x5A4 | 96.86% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring/scheduling permutation: func_001B1270's two float args are evaluated in  |
| func_00202D10 | 0x00202D10 | 0x5AC | 99.97% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 99.97% (mwcc233) — a single 2-instruction residual: target's `slti at,v0,3; bnez at,...` k |
| func_00221FC0 | 0x00221FC0 | 0x5B4 | 89.97% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 89.97% — the state-machine dispatch/body logic is fully recovered (all 12 case bodies byte |
| func_0021A500 | 0x0021A500 | 0x5BC | 89.22% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 89.22% — full logic recovered (state machine on e[4] driving a particle/twinkle-light init |
| func_001963A0 | 0x001963A0 | 0x5C4 | 96.59% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation: mwcc233 colors the switch(arg0[1]) dispatch temp in |
| func_00221060 | 0x00221060 | 0x5CC | 90.36% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order wall: mwcc picks $a4/$t0 for the switch-dispatch byte across the |
| func_00147B50 | 0x00147B50 | 0x5D4 | 99.25% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 99.25%, byte count matches target exactly (0x5D4). Two tiny residuals, both compiler-inter |
| func_0016EF50 | 0x0016EF50 | 0x5D4 | 90.09% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Compiler scheduling/register-choice wall (idiom-13 family): the drift-add pair (arg0+0xB0/ |
| func_0018B3E0 | 0x0018B3E0 | 0x5D4 | 97.06% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (state machine, all field offsets, calls, and idiom-15 relo |
| func_001A7280 | 0x001A7280 | 0x5F0 | 72.05% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 72.05% (233) -- body-level control flow, offsets, and call sequence recovered with reasona |
| func_00223C70 | 0x00223C70 | 0x614 | 99.67% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered; genuine residual is the documented D_00810E70/0x70003B7x d |
| func_00138C20 | 0x00138C20 | 0x618 | 99.64% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure/globals/float-constants fully recovered. Two residuals: (1) top dispatch de |
| func_002230A0 | 0x002230A0 | 0x650 | 98.90% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/every field/global access and all call-argument shapes are byte-identical. Sole resid |
| func_0020D930 | 0x0020D930 | 0x668 | 66.34% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation-ORDER permutation: target colors arg0->$s1 (high)/arg1->$s0 (low); thi |
| func_001C40B0 | 0x001C40B0 | 0x670 | 89.85% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | $at-vs-GPR compare, shared-epilogue edge case: every clamp-to-99 test's slti result lives  |
| func_001A6440 | 0x001A6440 | 0x690 | 88.58% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Not a genuine idiom-13 wall: body/logic fully recovered (segment-vs-collision-mesh sweep m |
| func_00217FA0 | 0x00217FA0 | 0x69C | 99.15% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered and verified byte-for-byte against the .s (weapon-reload FS |
| func_001936E0 | 0x001936E0 | 0x6AC | 94.99% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (melee-grab/lunge-attack FSM; real switch(kind) case values |
| func_001A3980 | 0x001A3980 | 0x6AC | 63.48% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Broad register-allocation/instruction-scheduling permutation across the whole 1708-byte bo |
| func_001A32C0 | 0x001A32C0 | 0x6B4 | 61.20% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order permutation (multiple saved-reg groups from the two nested passe |
| func_0016C6A0 | 0x0016C6A0 | 0x6C4 | 93.70% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | anim_clip_arbiter dead-float-argument scheduling + func_00224290 branch-likely tail schedu |
| func_0017A130 | 0x0017A130 | 0x6C8 | 93.94% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order wall (highest-vs-lowest saved reg for the sole long-lived param) |
| func_00219870 | 0x00219870 | 0x6D4 | 92.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered and byte-identical up to a register-allocation-order permut |
| func_001328D0 | 0x001328D0 | 0x6D8 | 96.65% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (96.6% on 233). Two residual classes, both genuine compiler |
| func_0020F950 | 0x0020F950 | 0x6D8 | 85.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | 3-state actor-tick dispatcher (mode 0 init / mode 1 setup / mode 2 render-config), gated b |
| func_00162DB0 | 0x00162DB0 | 0x6E4 | 91.18% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 91.18% mwcc233 (82.29% 991202). Body/structure fully recovered, all state-machine |
| func_001E3630 | 0x001E3630 | 0x6E4 | 93.92% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 93.92% mwcc233 (85.70% 991202). Body/structure/control-flow fully recovered acros |
| func_0016F600 | 0x0016F600 | 0x6EC | 79.81% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 79.81% mwcc233 (76.40% 991202). Logic and control flow fully recovered (validated |
| func_001764E0 | 0x001764E0 | 0x6F4 | 83.63% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | register-allocation-ORDER across the whole body: mwcc233 puts arg0 in s4 (target: s0), wit |
| func_001BECC0 | 0x001BECC0 | 0x6FC | 94.21% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | residual register-allocation / instruction-scheduling deltas (a couple of lh/sh reg-name s |
| func_00217090 | 0x00217090 | 0x71C | 97.30% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | compiler artifact (register coloring / scheduling) |
| func_001D30A0 | 0x001D30A0 | 0x724 | 92.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Bulk per-slot-array initializer (dynamic index shift = D_00275670->0x9c << 7, sibling of m |
| func_00226070 | 0x00226070 | 0x730 | 69.23% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Larger weapon-swap FSM (state on arg0+1, dispatched via 5-way beq chain matching switch-de |
| func_00140F80 | 0x00140F80 | 0x744 | 98.40% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Two residual compiler artifacts, logic/structure fully recovered: (1) clean-store idiom-13 |
| func_00218640 | 0x00218640 | 0x744 | 97.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation-order / instruction-scheduling permutation, same class as the sibling  |
| func_001459A0 | 0x001459A0 | 0x770 | 90.01% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Delay-slot/instruction-scheduling artifact repeated across the six capsule-test call sites |
| func_00165B60 | 0x00165B60 | 0x770 | 99.54% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/control-flow/data 100% byte-faithful except D_00275B40 (a pointer global near $gp): t |
| func_0017D080 | 0x0017D080 | 0x780 | 81.85% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring / parameter-register-choice permutation (a0-vs-a1 for a loop bound) plus |
| func_001E67C0 | 0x001E67C0 | 0x7A0 | 71.55% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Frame-size/register-pressure mismatch: this build colors one extra callee-saved integer re |
| func_00218D90 | 0x00218D90 | 0x7B8 | 94.05% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | residual: (a) fixed D_002821B0/B4/00282240 store-reorder mwcc always emits regardless of s |
| func_0017C860 | 0x0017C860 | 0x7DC | 82.66% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | VU0/FPU register-coloring and instruction-scheduling spread across many mula.s/madd.s sum- |
| func_001861C0 | 0x001861C0 | 0x7D8 | 62.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | large-function (7 saved regs, ~500 instructions, dense 0x700038xx GS/VU0 scratch traffic)  |
| func_001A2AE0 | 0x001A2AE0 | 0x7DC | 77.20% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Genuine register-coloring + branch-likely-sense scheduling artifact (not idiom-13/inverse- |
| func_001A1390 | 0x001A1390 | 0x7F0 | 76.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Same wall class as sibling func_001A1B80 (identical body/shape, only the kind threshold co |
| func_001A1B80 | 0x001A1B80 | 0x7F0 | 76.03% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Genuine register-coloring + branch-likely-sense scheduling artifact. Full logic/structure  |
| func_002177B0 | 0x002177B0 | 0x7F0 | 95.33% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | compiler artifact: register-allocation/coloring permutation (at-vs-v0/v1/a0 naming) throug |
| func_00139240 | 0x00139240 | 0x7A8 | 99.15% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | FP-companion register coloring on direct lwc1 loads (documented un-leverable class): in th |
| func_0014AFA0 | 0x0014AFA0 | 0x808 | 99.87% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Clean-store idiom-13 delay-slot fill (branch-target-advance): after the bnez guarding the  |
| func_001A5C30 | 0x001A5C30 | 0x808 | 96.33% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-coloring / RA-order permutation (body-shape identical): (1) FP temp coloring swap |
| func_001F3620 | 0x001F3620 | 0x80C | 99.92% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Single register-allocation swap (a0<->v1) on one 7-instruction pair: in the D_00275C44 sou |
| func_001735C0 | 0x001735C0 | 0x810 | 99.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 99.08% mwcc233 (991202 89.81% - idiom-13 nops + unsigned-char andi artifacts prov |
| func_002101C0 | 0x002101C0 | 0x83C | 87.91% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | NEARMISS 89.09% mwcc/991202 (233 87.91% - target FILLS branch slots with lui/li, so 991202 |
| func_001607D0 | 0x001607D0 | 0x850 | 97.08% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body byte-identical; SOLE residual class (15 uniform sites) = mwcc 2.3.3 speculates the li |
| func_00186A60 | 0x00186A60 | 0x860 | 96.43% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (521/536 instrs identical); 3 residual regions, all 2.3.1.0 |
| func_001E3D90 | 0x001E3D90 | 0x870 | 74.39% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Body/structure fully recovered (every block traced against the .s; m2c cross-checked); bes |
| func_001A0B10 | 0x001A0B10 | 0x878 | 95.37% | mwcc 2.3.3 (mwcps2-2.3.3-000906) | Register-allocation web permutation + list-scheduling residual (NEARMISS harvest; body and |
