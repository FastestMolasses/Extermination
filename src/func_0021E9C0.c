// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Player hit reaction 0x11 (player +0x04 = 2, +0x05 = 0x11): the reaction to
// AREA11 fan r2. The fan (overlay func_overlay_AREA11_008275F0) writes the
// damage type +0x0F = 6 and the pending damage +0x224 = 5.0; func_0021C440
// applies the damage and, while health remains, selects this reaction.
// Sub-state byte +0x06:
//
//   0  start: sound 0x154 (func_001FBD50, radius 300), a forced small-motor
//      rumble at 0xC0 for 5 frames (func_001B61C0), sub-state 1 with the byte
//      +0x07 cleared, start clip 0x20 (func_001749A0), and zero the speed
//      +0x38, the previous root travel +0x21C and +0x2EC.
//   1  knock-back: while the clip-end bit (+0x200 & 0x1000) is clear, the
//      speed +0x38 is this frame's change in the root bone's +0x08 value
//      (D_00275B40[0], the clip's root travel; +0x21C keeps last frame's
//      value) and func_00178B90 moves the player by that speed along the
//      heading +0xC4, with collision. At the clip end it clears the damage
//      type byte +0x0F, arms the post-hit invulnerability countdown +0x20E
//      to 60 frames, clears +0x25C (so func_0017C540 hands back to idle,
//      +0x05 = 0) and returns control to the player.
//
// Every frame then runs the shared tail func_00179880(player, player +
// 0x2EC) and func_00175900(player, 1).
extern void func_001FBD50(char *obj, int sound, int flat, float radius);
extern void func_001B61C0(int big, int small, int frames, int force);
extern void func_001749A0(char *player, int clip, int a2, float f);
extern void func_00179880(char *player, char *p);
extern void func_00175900(char *player, int a1);
extern void func_0017C540(char *player, int a1);
extern void func_00178B90(char *player, int collide);
extern char **D_00275B40;

void func_0021E9C0(char *player) {
    switch (*(unsigned char *)(player + 6)) {
    case 0:
        func_001FBD50(player, 0x154, 0, 300.0f);
        func_001B61C0(0, 0xC0, 5, 1);
        *(unsigned char *)(player + 6) = *(unsigned char *)(player + 6) + 1;
        *(char *)(player + 7) = 0;
        func_001749A0(player, 0x20, 0, 1.0f);
        *(int *)(player + 0x38) = 0;
        *(int *)(player + 0x21C) = 0;
        *(int *)(player + 0x2EC) = 0;
        break;
    case 1:
        if (*(int *)(player + 0x200) & 0x1000) {
            *(char *)(player + 0xF) = 0;
            *(short *)(player + 0x20E) = 60;
            *(char *)(player + 0x25C) = 0;
            func_0017C540(player, 1);
        } else {
            *(float *)(player + 0x38) = *(float *)(D_00275B40[0] + 8) - *(float *)(player + 0x21C);
            *(float *)(player + 0x21C) = *(float *)(D_00275B40[0] + 8);
            func_00178B90(player, 1);
        }
        break;
    }
    func_00179880(player, player + 0x2EC);
    func_00175900(player, 1);
}
