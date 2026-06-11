// INCLUDE_ASM func_001BBE40  (vram 0x001BBE40, 660 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// ---------------------------------------------------------------------------
// 2026-06-10 attempt: WALL-BLOCKED at 91.5% (151/165 rows, 4 attempts).
// Door transit KICKOFF (runs while door +0x0B bit2 set; docs/FINDINGS.md
// "DOOR SCRIPTS DECODED" + s17). Semantics fully decoded in the C below:
// side latch from |normalize(atan2(player-door) - door_yaw)| <= pi/2,
// patch the shared script records (player anim id 0x45/0x43 open or
// 0x46/0x44 locked, door clip 2/0 or 3/1, wait 90/70 frames, sound pair
// via func_001BBD60), snap player yaw to the door normal, build the
// walk-through staging point spad 0x700038A0 = door_pos -/+ 5*[cos,sin]
// (door_yaw) then -= 5*[sin,cos](player_yaw), MOVE-TO (func_00182F90),
// queue script D_0024DE40 (mode 0 = unlocked) / D_0024DEC0 (mode 1 =
// locked) on the anim block and pump it (func_001BA1F0). Returns
// 1 = kicked off, 0 = not armed.
//
// All 14 residual rows are documented wall families:
//  - dead-`b`-dup: the dead `addiu v1,1` at +0x3F8 (dup of the bc1f
//    delay-slot const) — mwcc always coalesces (+3 knock-on b-target rows);
//  - per-arm `lwc1 f1,0xC4(s3)` dup'd into the arms' `b`-delay slots (CW
//    merge-head duplication; mwcc emits the load once at the merge, nop in
//    the slot) x2;
//  - prologue fill choice: CW [addiu s0,%lo(player) early ... beqz; paddub
//    s3,a0 (slot)]; mwcc [paddub s3 early ... beqz; addiu s0 (slot)];
//  - wall #13: `bnez s1; nop` before the script-queue arms (mwcc fills with
//    the safe fall-through lui);
//  - call-arg setup order: CW [lui; addiu a1,%lo(script); jal; paddub a0
//    (slot)]; mwcc [lui; paddub a0; jal; addiu a1 (slot)] x2 arms (+ the
//    paddub a0,s0 placement at func_00182F90) — same family as
//    func_001B99F0 datum (d).
// POSITIVE datums from this attempt (recorded in PROGRESS):
//  - do NOT copy params to locals here: `d = door; b = blk; m = mode;`
//    INVERTS mwcc's s1/s2 choice; BARE param usage allocates a0->s3,
//    a1->s2, a2->s1 with the body local in s0 — exactly CW's reversed-
//    by-declaration mapping (counter-datum to the anim_sample_bones
//    param-copy trick: try bare params FIRST when the target mapping is
//    a0->sN..aN->s1).
//  - `volatile` on the five script patch globals D_0024DC14/DC54/DC8C/
//    DCD4/DD14 pinned CW's store order AND moved the shared `1` constant
//    to v1 (2nd confirmation that volatile-per-object pins short blocks).
//  - float literals (1.5707963705062866f etc.) compile to CW's exact
//    lui/ori -> mtc1; `*(int *)&f = K` instead allocates a STACK slot
//    (sw/lwc1 roundtrip) — only use the bit-pattern form for INTEGER
//    stores of float constants (e.g. D_0024DC8C = 0x42B40000 and the
//    spad 0x700038AC = 0x3F800000 store, both matched).
//
// Best attempt (restore + `// CFLAGS: -O4,p -sdatathreshold 0` to verify):
//
// extern unsigned char D_008102B0[];
// extern volatile int D_0024DC14, D_0024DC54, D_0024DC8C;
// extern volatile int D_0024DCD4, D_0024DD14;
// extern char D_0024DC40[], D_0024DE40[], D_0024DEC0[];
// extern float D_700038A0[];
// extern float func_001B1240(float *pos, float x, float z);
// extern float func_001B1470(float ang);
// extern float func_0011DF78(float ang);   /* fabs  */
// extern float func_0011DE90(float ang);   /* cos — s45 trig-label fix */
// extern float func_0011E2A8(float ang);   /* sin — s45 trig-label fix */
// extern void func_001BBD60(void *door, void *rec);
// extern void func_00182F90(void *player, void *pt);
// extern void func_001BA1A0(void *blk, void *script);
// extern void func_001BA1F0(void *door);
//
// int func_001BBE40(unsigned char *door, unsigned char *blk, int mode) {
//     unsigned char *player;
//     float ang, yawv;
//     char *rec, *scr;
//     float *pt;
//
//     player = D_008102B0;
//     if (door[0xB] & 4) {
//         ang = func_0011DF78(func_001B1470(
//             func_001B1240((float *)(door + 0xB0), *(float *)(player + 0xA0),
//                           *(float *)(player + 0xA8)) - *(float *)(door + 0xC4)));
//         if (ang <= 1.5707963705062866f) {           /* front side */
//             *(short *)(door + 0x2E) = 0;
//             if (mode == 0) {
//                 D_0024DC14 = 0x45;                  /* player anim id  */
//                 D_0024DC54 = 2;                     /* door clip       */
//                 D_0024DC8C = 0x42B40000;            /* wait 90.0f      */
//                 rec = D_0024DC40;
//                 func_001BBD60(door, rec);           /* sound pair      */
//                 yawv = *(float *)(door + 0xC4);
//             } else {
//                 D_0024DCD4 = 0x46;
//                 D_0024DD14 = 3;
//                 yawv = *(float *)(door + 0xC4);
//             }
//             *(float *)(player + 0xC4) =
//                 func_001B1470(3.1415927410125732f + yawv);
//         } else {                                    /* back side */
//             *(short *)(door + 0x2E) = 1;
//             if (mode == 0) {
//                 D_0024DC14 = 0x43;
//                 D_0024DC54 = 0;
//                 D_0024DC8C = 0x428C0000;            /* wait 70.0f */
//                 rec = D_0024DC40;
//                 func_001BBD60(door, rec);
//                 yawv = *(float *)(door + 0xC4);
//             } else {
//                 D_0024DCD4 = 0x44;
//                 D_0024DD14 = 1;
//                 yawv = *(float *)(door + 0xC4);
//             }
//             *(float *)(player + 0xC4) = func_001B1470(yawv);
//         }
//         *(volatile float *)0x700038A0 = *(float *)(door + 0xB0)
//             - 5.0f * func_0011DE90(*(float *)(door + 0xC4));
//         *(volatile float *)0x700038A4 = *(float *)(player + 0xA4);
//         *(volatile float *)0x700038A8 = *(float *)(door + 0xB8)
//             + 5.0f * func_0011E2A8(*(float *)(door + 0xC4));
//         *(volatile int *)0x700038AC = 0x3F800000;
//         *(volatile float *)0x700038A0 = *(volatile float *)0x700038A0
//             - 5.0f * func_0011E2A8(*(float *)(player + 0xC4));
//         *(volatile float *)0x700038A4 = *(volatile float *)0x700038A4;
//         *(volatile float *)0x700038A8 = *(volatile float *)0x700038A8
//             - 5.0f * func_0011DE90(*(float *)(player + 0xC4));
//         pt = D_700038A0;
//         func_00182F90(player, pt);                  /* walk-through */
//         if (mode == 0) {
//             scr = D_0024DE40;                       /* OPEN script   */
//             func_001BA1A0(blk, scr);
//         } else {
//             scr = D_0024DEC0;                       /* LOCKED script */
//             func_001BA1A0(blk, scr);
//         }
//         func_001BA1F0(door);                        /* pump */
//         return 1;
//     }
//     return 0;
// }
// ---------------------------------------------------------------------------
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001BBE40.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
