// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Proximity gate: refresh the player-relative vector via func_001028D0, then pick this
// entity's reaction state from its distance d = len(x,y,z) — bail out past 70 units,
// randomize a close-range (<30.1) or mid-range (<60.1) response through func_00128600,
// else fall back to state 2; clears the two sub-state bytes and returns 1 when armed.

extern int func_001028D0(void *a, void *b, void *c);
extern float func_0011E748(float d);
extern int func_00128600(int a);
extern short D_0028A9A0;
extern int D_00810350;
extern int D_008104E0;
extern float D_70003600;

int func_00128640(char *arg0) {
    float y;
    float x;
    float z;
    float d;
    int r;

    if (*(unsigned char *)0x70003B8D != 0) {
        return 0;
    }
    if (D_0028A9A0 != 0) {
        return 0;
    }
    if (D_008104E0 == 0x28) {
        return 0;
    }
    func_001028D0(&D_70003600, &D_00810350, arg0 + 0xB0);
    x = *(float *)0x70003600;
    y = *(float *)0x70003604;
    z = *(float *)0x70003608;
    d = func_0011E748(((x * x) + (y * y)) + (z * z));
    if (d > 70.0f) {
        return 0;
    }
    if (d < 30.1f) {
        r = func_00128600(4);
        if (r == 0) {
            *(char *)(arg0 + 5) = 2;
        } else if (r == 1) {
            *(char *)(arg0 + 5) = 3;
        } else {
            *(char *)(arg0 + 5) = 5;
        }
    } else if (d < 60.1f) {
        r = func_00128600(0);
        if (r == 0) {
            *(char *)(arg0 + 5) = 2;
        } else if (r == 1) {
            *(char *)(arg0 + 5) = 4;
        } else {
            *(char *)(arg0 + 5) = 5;
        }
    } else {
        *(char *)(arg0 + 5) = 2;
    }
    *(char *)(arg0 + 6) = 0;
    *(char *)(arg0 + 7) = 0;
    return 1;
}
