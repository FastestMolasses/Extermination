// CFLAGS: -O4,p -sdatathreshold 0
//
// One steer tick on the actor at `self`: compute a goal heading from the
// actor's launch/position point (+0xB0) and the world reference pair
// (D_00810350, D_00810358) via func_001B1240, then ease the current yaw
// (+0xC4) toward that goal at a fixed rate of ~0.0436 rad/tick
// (0x3D32B8C3f, ~pi/72) via the yaw-steer helper func_001B12B0(goal, cur,
// rate) -> new yaw, storing the result back to +0xC4.
//
// Matches on BOTH builds; 991202 preferred. Verified objdiff 100.0 vs
// build/expected/func_0012E260.o.
extern float func_001B1240(unsigned char *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern float D_00810350;
extern float D_00810358;

void func_0012E260(unsigned char *self) {
    float goal;

    goal = func_001B1240(self + 0xB0, D_00810350, D_00810358);
    *(float *)(self + 0xC4) =
        func_001B12B0(goal, *(float *)(self + 0xC4), 0.04363323375582695f);
}
