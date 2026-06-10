// Door sub-state 4 (post-open hold): advance the door articulation clip one
// step (result -> anim block +0xE current time), then commit the room/area
// transition request (func_001BC150 reads the door id at actor+0x34 and
// writes the D_008106B5..B8 transition block). Caller func_001BC350 then
// advances the door to sub-state 5 (await transition completion).
extern short anim_advance_time(void *self, float step);
extern void func_001BC150(void *self);

void func_001BC240(unsigned char *self, unsigned char *blk) {
    *(short *)(blk + 0xE) = anim_advance_time(self, 1.0f);
    func_001BC150(self);
}
