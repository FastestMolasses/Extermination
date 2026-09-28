// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x008241B0 (splat/link name 00824170; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: follows seven nodes of the record at +0x24 (indices at 0x828A40,
//  node pointers at +0x110): +5 1 and +5 2 (every 10th count) spawn effect
//  0x80000015 at each node's x/z with y = -65; +5 3 spawns it where a node
//  crosses below y = -54; 0x63 ends (state 3). Two timers at +0x40 drive
//  pairs from 0x828A30 (func_00102918 / func_001CCF70), stepping 0.004 and
//  wrapping past 2.0.
// Matching: every loop shares the one counter `i`; a second counter variable
//  changes the argument-register choice in the state 0 node-height loop.
typedef struct {
    float h[7];         /* 0x00 */
    int pad1C[9];       /* 0x1C */
    float t[2];         /* 0x40 */
    int pad48[14];      /* 0x48 */
    int seed;           /* 0x80 */
    int counter;        /* 0x84 */
} Fx;
#define NODE(parent, k) (*(unsigned char **)((parent) + 0x110 + D_overlay_AREA00_00828A40[k] * 4))
extern int D_overlay_AREA00_00828A40[7];
extern int D_overlay_AREA00_00828A30[2][2];
extern float D_700038A0[4];
extern char D_700036A0[];
extern char D_700036D0[];
extern float D_70003A20;
extern int func_00122BB8(void);
extern char *func_001F00A0(int id, void *pos, void *rot, int a);
extern void func_001029C0(void *m);
extern void func_00102948(void *dst, void *src);
extern void func_00102918(void *a, void *b, void *c);
extern int func_001CCF70(void *p);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00824170(unsigned char *self) {
    int seed;
    int mode;
    Fx *fx = (Fx *)(self + 0x1F0);
    unsigned char *parent = *(unsigned char **)(self + 0x24);
    int i;
    char *e;
    int node;
    float r;
    switch (self[4]) {
    case 0:
        for (i = 0; i < 2; i++) {
            fx->t[i] = -1.5f * ((float)i / 2.0f);
        }
        for (i = 0; i < 7; i++) {
            fx->h[i] = *(float *)(NODE(parent, i) + 0xC4);
        }
        fx->seed = func_00122BB8();
        fx->counter = 0;
        self[4] = 1;
        self[5] = 0;
        self[6] = 0;
    case 1:
        if (parent[4] == 3) {
            self[4] = 3;
            break;
        }
        if (self[6] != self[5]) {
            self[6] = self[5];
            fx->counter = 0;
        }
        switch (self[5]) {
        case 0:
            break;
        case 1:
            for (i = 0; i < 7; i++) {
                D_700038A0[0] = *(float *)(NODE(parent, i) + 0xC0);
                D_700038A0[2] = *(float *)(NODE(parent, i) + 0xC8);
                D_700038A0[1] = -65.0f;
                D_700038A0[3] = 1.0f;
                e = func_001F00A0(0x80000015, D_700038A0, D_700038A0, 0);
                if (e != 0) {
                    func_001029C0(e + 0xD0);
                    func_00102948(e + 0x100, D_700038A0);
                }
            }
            self[5] = 0;
            break;
        case 2:
            if (fx->counter % 10 == 0) {
                for (i = 0; i < 7; i++) {
                    D_700038A0[0] = *(float *)(NODE(parent, i) + 0xC0);
                    D_700038A0[2] = *(float *)(NODE(parent, i) + 0xC8);
                    D_700038A0[1] = -65.0f;
                    e = func_001F00A0(0x80000015, D_700038A0, D_700038A0, 0);
                    if (e != 0) {
                        func_001029C0(e + 0xD0);
                        func_00102948(e + 0x100, D_700038A0);
                    }
                }
            }
            break;
        case 3:
            for (i = 0; i < 7; i++) {
                if (*(float *)(NODE(parent, i) + 0xC4) < -54.0f) {
                    if (!(fx->h[i] <= -54.0f)) {
                        D_700038A0[0] = *(float *)(NODE(parent, i) + 0xC0);
                        D_700038A0[2] = *(float *)(NODE(parent, i) + 0xC8);
                        D_700038A0[1] = -65.0f;
                        D_700038A0[3] = 1.0f;
                        e = func_001F00A0(0x80000015, D_700038A0, D_700038A0, 0);
                        if (e != 0) {
                            func_001029C0(e + 0xD0);
                            func_00102948(e + 0x100, D_700038A0);
                        }
                    }
                }
            }
            break;
        case 0x63:
            self[4] = 3;
            break;
        }
        fx->counter++;
        for (i = 0; i < 7; i++) {
            fx->h[i] = *(float *)(NODE(parent, i) + 0xC4);
        }
        seed = fx->seed;
        for (i = 0; i < 2; i++) {
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            *(float *)0x70003A20 = r;
            if (!(fx->t[i] < 0.0f)) {
                node = D_overlay_AREA00_00828A30[i][0];
                mode = D_overlay_AREA00_00828A30[i][1];
                func_001029C0(D_700036A0);
                func_00102918(D_700036A0, D_700036A0, *(unsigned char **)(parent + 0x110 + node * 4) + 0xC0);
                func_001CCF70(D_700036D0);
                switch (mode) {
                case 0:
                case 1:
                    fx->t[i] += 0.004f;
                    break;
                case 2:
                    fx->t[i] += 0.004f;
                    break;
                }
                if (!(fx->t[i] <= 2.0f)) {
                    fx->t[i] -= 1.0f;
                }
            } else {
                fx->t[i] += 0.1f;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
