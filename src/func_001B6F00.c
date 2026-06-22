// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Updates a global animation/camera state: stores (obj->f0xC4 + delta) through
// func_001B1470 into D_00810374, fills the scratchpad VU buffer at D_700038A0
// from obj->0xD0 via func_001026A0, mirrors D_00810354 into the raw scratchpad
// address 0x700038A4, then submits the buffer with func_00182F90.

extern float func_001B1470(float x);
extern void func_001026A0(float *dst, char *src, int n);
extern void func_00182F90(void *a, float *b);

extern float D_700038A0;
extern float D_00810354;
extern float D_00810374;
extern char D_008102B0[];

void func_001B6F00(char *arg0, int arg1, float fparg0) {
    D_00810374 = func_001B1470(*(float *)(arg0 + 0xC4) + fparg0);
    func_001026A0(&D_700038A0, arg0 + 0xD0, arg1);
    *(volatile float *)0x700038A4 = D_00810354;
    func_00182F90(D_008102B0, &D_700038A0);
}
