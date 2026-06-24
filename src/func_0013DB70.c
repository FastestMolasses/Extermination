// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Two-phase actor animation/steer tick keyed on the state byte self[6].
// Phase 0 (first entry): bump the state to 1, initialise the actor's anim
// fields at arg1+0x40..0x50 (set +0x40 = 1.0f, zero +0x48/+0x4C/+0x50), then
// kick off clip 0xB via anim_clip_init(self, 0xB, 15.0f, 0.0f) and
// func_001FBD50(self, 0x808, 0, 300.0f).
// Phase 1 (running): compute a goal heading from the actor launch point
// (self+0xB0) and world reference pair (D_00810360, D_00810368) via
// func_001B1240, stash it at arg1+0x44, then ease the current yaw (self+0xC4)
// toward it at ~0.0175 rad/tick (1 degree, 0x3C8EFA35) via
// func_001B12B0(goal, cur, rate), storing back to self+0xC4. If the actor
// flag word at arg1+0x30 has bit 0x1000 set, finish: self[5]=4, self[6]=0.
// Any other state value just returns.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// state-byte switch dispatch + the conditional-branch delay-slot nops match
// under 2.3.3; 991202 fills those slots differently (89.7%). Verified objdiff
// 100.0 vs build/expected/func_0013DB70.o.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001FBD50(int self, int a, int b, float c);
extern float func_001B1240(unsigned char *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern float D_00810360;
extern float D_00810368;

void func_0013DB70(unsigned char *self, unsigned char *arg1) {
    switch (self[6]) {
    case 0:
        self[6] = self[6] + 1;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(int *)(arg1 + 0x48) = 0;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init((int)self, 0xB, 15.0f, 0.0f);
        func_001FBD50((int)self, 0x808, 0, 300.0f);
        break;
    case 1:
        *(float *)(arg1 + 0x44) = func_001B1240(self + 0xB0, D_00810360, D_00810368);
        *(float *)(self + 0xC4) =
            func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(self + 0xC4), 0.017453292f);
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            self[5] = 4;
            self[6] = 0;
        }
        break;
    }
}
