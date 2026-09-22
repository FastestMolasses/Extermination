// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Align the player to an owner's transformed local interaction point.
// Wrap owner yaw + offset into the player's C4 yaw mirror, transform the
// supplied four-float point by owner+D0, retain current ground Y, then
// shift player position mirrors and capture Euler via00182F90.

extern float func_001B1470(float x);
extern void func_001026A0(float *dst, const float *matrix, const float *point);
extern void func_00182F90(void *a, float *b);

extern float D_700038A0;
extern float D_00810354;
extern float D_00810374;
extern char D_008102B0[];

void func_001B6F00(char *owner, const float *local_point, float yaw_offset) {
    D_00810374 = func_001B1470(*(float *)(owner + 0xC4) + yaw_offset);
    func_001026A0(&D_700038A0, (float *)(owner + 0xD0), local_point);
    *(volatile float *)0x700038A4 = D_00810354;
    func_00182F90(D_008102B0, &D_700038A0);
}
