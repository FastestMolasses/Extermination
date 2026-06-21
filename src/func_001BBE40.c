// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SEMANTICS (s17, FINDINGS "DOOR SCRIPTS DECODED"): door transit KICKOFF (runs
// while door +0x0B bit2 set). Side latch from |normalize(atan2(player-door) -
// door_yaw)| <= pi/2, patch the shared script records (player anim id 0x45/0x43
// open or 0x46/0x44 locked, door clip 2/0 or 3/1, wait 90/70 frames, sound pair
// via func_001BBD60), snap player yaw to the door normal, build the walk-through
// staging point spad 0x700038A0 = door_pos -/+ 5*[cos,sin](door_yaw) then -=
// 5*[sin,cos](player_yaw), MOVE-TO (func_00182F90), queue script D_0024DE40
// (mode 0 = unlocked) / D_0024DEC0 (mode 1 = locked) on the anim block and pump it
// (func_001BA1F0). Returns 1 = kicked off, 0 = not armed.
// KEY MATCH NOTE: read *(float*)(door+0xC4) directly at the merge call (do NOT
// stash it in a `yawv` local) — that lets the b-to-merge delay slots carry the
// loads, matching CW's merge-head duplication.
extern unsigned char D_008102B0[];
extern volatile int D_0024DC14, D_0024DC54, D_0024DC8C;
extern volatile int D_0024DCD4, D_0024DD14;
extern char D_0024DC40[], D_0024DE40[], D_0024DEC0[];
extern float D_700038A0[];
extern float func_001B1240(float *pos, float x, float z);
extern float func_001B1470(float ang);
extern float func_0011DF78(float ang);
extern float func_0011DE90(float ang);
extern float func_0011E2A8(float ang);
extern void func_001BBD60(void *door, void *rec);
extern void func_00182F90(void *player, void *pt);
extern void func_001BA1A0(void *blk, void *script);
extern void func_001BA1F0(void *door);

int func_001BBE40(unsigned char *door, unsigned char *blk, int mode) {
    unsigned char *player;
    float ang;
    char *rec, *scr;
    float *pt;

    player = D_008102B0;
    if (door[0xB] & 4) {
        ang = func_0011DF78(func_001B1470(
            func_001B1240((float *)(door + 0xB0), *(float *)(player + 0xA0),
                          *(float *)(player + 0xA8)) - *(float *)(door + 0xC4)));
        if (ang <= 1.5707963705062866f) {
            *(short *)(door + 0x2E) = 0;
            if (mode == 0) {
                D_0024DC14 = 0x45;
                D_0024DC54 = 2;
                D_0024DC8C = 0x42B40000;
                rec = D_0024DC40;
                func_001BBD60(door, rec);
            } else {
                D_0024DCD4 = 0x46;
                D_0024DD14 = 3;
            }
            *(float *)(player + 0xC4) =
                func_001B1470(3.1415927410125732f + *(float *)(door + 0xC4));
        } else {
            *(short *)(door + 0x2E) = 1;
            if (mode == 0) {
                D_0024DC14 = 0x43;
                D_0024DC54 = 0;
                D_0024DC8C = 0x428C0000;
                rec = D_0024DC40;
                func_001BBD60(door, rec);
            } else {
                D_0024DCD4 = 0x44;
                D_0024DD14 = 1;
            }
            *(float *)(player + 0xC4) = func_001B1470(*(float *)(door + 0xC4));
        }
        *(volatile float *)0x700038A0 = *(float *)(door + 0xB0)
            - 5.0f * func_0011DE90(*(float *)(door + 0xC4));
        *(volatile float *)0x700038A4 = *(float *)(player + 0xA4);
        *(volatile float *)0x700038A8 = *(float *)(door + 0xB8)
            + 5.0f * func_0011E2A8(*(float *)(door + 0xC4));
        *(volatile int *)0x700038AC = 0x3F800000;
        *(volatile float *)0x700038A0 = *(volatile float *)0x700038A0
            - 5.0f * func_0011E2A8(*(float *)(player + 0xC4));
        *(volatile float *)0x700038A4 = *(volatile float *)0x700038A4;
        *(volatile float *)0x700038A8 = *(volatile float *)0x700038A8
            - 5.0f * func_0011DE90(*(float *)(player + 0xC4));
        pt = D_700038A0;
        func_00182F90(player, pt);
        if (mode == 0) {
            scr = D_0024DE40;
            func_001BA1A0(blk, scr);
        } else {
            scr = D_0024DEC0;
            func_001BA1A0(blk, scr);
        }
        func_001BA1F0(door);
        return 1;
    }
    return 0;
}
