// INCLUDE_ASM func_00153B50  (vram 0x00153B50, 844 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved — docs/FINDINGS.md "ENEMY AI ARCHITECTURE" §5):
// state-2 (hurt/death) handler of the func_00153950 enemy (AREA15's
// phase-gated boss), the canonical +0x36 damage-mailbox consumer.
// Dispatch on sub-state +0x05:
//   0 HP(+0x34) -= dmg(+0x36); dead -> sub-state 2, knockback vel/accel
//     -0.01f/-0.03f raw-int into ctx, kill flag D_008107FB = 2, sound
//     0x7D8, anim 0x34; alive -> sub-state++, zero ctx vel block, sound
//     0x7D4, anim 0x37 (flinch).
//   1 on clip-done bit 0x1000: lifecycle(+0x04)=1, sub=0, ping +0x00=1,
//     clear mailbox, anim 0x33 (re-enter active).
//   2 death fall: vy += ay (clamp -5.0 raw 0xC0A00000), y += vy,
//     x/z += speed * sin/cos(yaw) (func_0011E2A8/func_0011DE90), build
//     spad vec4s D_700038A0 = {0,-2,0,1}, += pos via func_001028B8,
//     D_700038B0 = {0,vy,0,1}; floor probe func_0019AB20(self, dir,
//     ofs, 0x80000007) -> on touch-down sub++, sound 0x7D5, anim 0x36.
//   3 on clip-done: sub=4, ctx+0x10=0, sound 0x7D7, gore FX
//     func_001EFE00(0x8000001E, self); FX returns 0 -> lifecycle 3.
//   4 idle.  Tail: ctx status = anim_advance_time(self, 1.0f), then
//     func_00131ED0 / func_001C68C0 / func_001B17A0, +0x4C hook if set.
//
// DECOMP ATTEMPT 2026-06-10 — wall-blocked at 91.33% (best, semantics
// verified). What MATCHED (new idiom data, recorded in PROGRESS): a
// plain C `switch` on an idiom-16c fake-3rd-param (`state` pinned to
// $a2, read `state = self->sub05`) reproduces CW's beq+nop sparse chain
// EXACTLY — same reverse-order compares, same constant registers
// (v0,v0,a1,v1), the cross-block case-constant REUSE (`sb a1,0x5(s1)` /
// `sb a1,D_008107FB` consuming the chain's `addiu a1,zero,2`;
// `sb v1,...` consuming the 1), the dup'd common-tail `lui` in the
// default `b`'s delay slot, and `addiu v0,a2,1` in case 3 (the switch
// register stays live into the case body). Idiom 12c (clip id as own
// statement: `clip = 0x34; anim_clip_init(s, clip, 10.0f, 0.0f);`)
// fixed all three anim-call arg orders (li a1 before jal, paddub a0,s1
// in the slot). A `volatile` +0x4C hook field keeps mwcc from filling
// the tail `beqz` slot with the hook load (lw made unsafe).
//
// UNFIXABLE residual (16 rows, all documented wall families):
//  - wall #13 x4: chain slots after beq(3)/beq(2) (candidate = next
//    chain const addiu) and case 3's beqz/bnez slots (candidates
//    `addiu v0,a2,1` / `addiu v0,zero,3`) — mwcc fills, CW left nops.
//  - f13-before-f12 mtc1 swap in case 1 (anim 0x33 call) — the known
//    reversed-float-constant near-miss; the case 0/2 calls match.
//  - shadow-fill hoist x4 in case 2: mwcc lifts `lui %hi(D_700038A0)`,
//    `addiu a2,s1,0xb0`, `lui at,0x7000`, `addiu a2,%lo(D_700038B0)`
//    1-3 rows above CW's placement.
//  - prologue-hoist-family stall fill: mwcc parks `lui v0,0x4396` inside
//    the `lui at,%hi(D_008107FB)` / `sb` gap; CW kept it after the sb.
//  - case-2 func_001FBD50 arg order: CW [addiu a1][paddub a0][sb sub05]
//    [jal][slot paddub a2]; mwcc [paddub a0][addiu a1][paddub a2][jal]
//    [slot sb]. Falsified: volatile sub05 (no effect).
//
// Best attempt (91.33%) — keep for retry if a #13 crack ever lands:
//   typedef struct Actor153 { u8 ev00; char f01,f02,f03; u8 state04;
//     u8 sub05; char pad06[0x2E]; s16 hp34; s16 dmg36; char pad38[0x14];
//     int (* volatile post4C)(struct Actor153 *); char pad50[0x60];
//     float posB0, posB4, posB8; char padBC[8]; float yawC4; } Actor153;
//   typedef struct Ctx153 { int status00; float vy04, ay08, speed0C;
//     u8 f10; } Ctx153;
//   extern u8 D_008107FB; extern int D_700038A0[], D_700038B0[];
//   void func_00153B50(Actor153 *self, Ctx153 *ctx, int state) {
//       Ctx153 *c; Actor153 *s; int hp; int clip;
//       state = self->sub05; c = ctx; s = self;
//       switch (state) {
//       case 0:
//           hp = s->hp34 - s->dmg36; s->hp34 = hp;
//           if ((short)hp <= 0) {
//               s->sub05 = 2; c->vy04 = 0;
//               *(int *)&c->ay08 = 0xBC23D70A;
//               *(int *)&c->speed0C = 0xBCF5C28F;
//               D_008107FB = 2;
//               func_001FBD50(self, 0x7D8, 0, 300.0f);
//               clip = 0x34; anim_clip_init(s, clip, 10.0f, 0.0f);
//           } else {
//               s->sub05 += 1;
//               c->speed0C = 0; c->ay08 = 0; c->vy04 = 0;
//               func_001FBD50(self, 0x7D4, 0, 300.0f);
//               clip = 0x37; anim_clip_init(s, clip, 10.0f, 0.0f);
//           }
//           break;
//       case 1:
//           if (c->status00 & 0x1000) {
//               s->state04 = 1; s->sub05 = 0; s->ev00 = 1; s->dmg36 = 0;
//               anim_clip_init(self, 0x33, 10.0f, 0.0f);
//           }
//           break;
//       case 2:
//           c->vy04 += c->ay08;
//           if (c->vy04 < -5.0f) {
//               *(int *)&c->vy04 = 0xC0A00000; c->ay08 = 0;
//           }
//           s->posB4 += c->vy04;
//           s->posB0 += c->speed0C * func_0011E2A8(s->yawC4);
//           s->posB8 += c->speed0C * func_0011DE90(s->yawC4);
//           *(volatile int *)0x700038A0 = 0;
//           *(volatile int *)0x700038A4 = 0xC0000000;
//           *(volatile int *)0x700038A8 = 0;
//           *(volatile int *)0x700038AC = 0x3F800000;
//           func_001028B8(D_700038A0, D_700038A0, &s->posB0);
//           *(volatile int *)0x700038B0 = 0;
//           *(volatile float *)0x700038B4 = c->vy04;
//           *(volatile int *)0x700038B8 = 0;
//           *(volatile int *)0x700038BC = 0x3F800000;
//           if (func_0019AB20(s, D_700038A0, D_700038B0, 0x80000007)) {
//               s->sub05 += 1;
//               func_001FBD50(s, 0x7D5, 0, 300.0f);
//               clip = 0x36; anim_clip_init(s, clip, 10.0f, 0.0f);
//           }
//           break;
//       case 3:
//           if (c->status00 & 0x1000) {
//               s->sub05 = state + 1; c->f10 = 0;
//               func_001FBD50(self, 0x7D7, 0, 300.0f);
//               if (func_001EFE00(0x8000001E, s) == 0) s->state04 = 3;
//           }
//           break;
//       case 4: break;
//       }
//       c->status00 = anim_advance_time(s, 1.0f);
//       func_00131ED0(s); func_001C68C0(s);
//       if (func_001B17A0(s) != 0) s->post4C(s);
//   }
//   // protos: func_001FBD50(void*,int,int,float);
//   // anim_clip_init(void*,int,float,float); anim_advance_time(void*,float);
//   // float func_0011E2A8(float); float func_0011DE90(float);
//   // func_001028B8(int*,int*,float*); func_0019AB20(void*,int*,int*,int);
//   // func_001EFE00(int,void*); CFLAGS: -O4,p -sdatathreshold 0
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_00153B50.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
