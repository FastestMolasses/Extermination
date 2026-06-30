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
