// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00824910 (splat/link name 008248D0; overlay
// code is linked 0x40 below where it runs), 0x1A8 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Covers the splat pieces 008248D0, 00824910 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
// Role: 18 trigger records at 0x827350 (0x20 bytes: id, done flag,
//  position at +0x10). Each record that is not done fires once +0xB0 passes
//  its x: id 0x80000001 also spawns 0x80000013 at the position + (0, 10, 0),
//  id 0x80000002 also spawns 0x8000002F there; then func_001EFD20(id, pos),
//  done = 1, and a func_001F02C0 sound 0x449/0x44A/0x44B (rand % 3) at 200.
typedef struct {
    int id;
    int done;
    int pad[2];
    float pos[4];
} Trigger;
typedef struct {
    float x, y, z, w;
} Vec4;
extern Trigger D_overlay_AREA02_00827350[];
extern char *func_001EFD20(int id, void *a);
extern int func_00122BB8(void);
extern void func_001F02C0(float *, int, float);

void func_overlay_AREA02_008248D0(unsigned char *self) {
    Vec4 a;
    Vec4 b;
    unsigned int i;
    Trigger *t;

    for (i = 0, t = D_overlay_AREA02_00827350; i < 18; i++, t++) {
        if (t->done != 0) {
            continue;
        }
        if (*(float *)(self + 0xB0) <= t->pos[0]) {
            continue;
        }
        if (t->id == 0x80000001) {
            a.x = t->pos[0];
            a.y = 10.0f + t->pos[1];
            a.z = t->pos[2];
            a.w = 1.0f;
            func_001EFD20(0x80000013, &a);
        }
        if (t->id == 0x80000002) {
            b.x = t->pos[0];
            b.y = 10.0f + t->pos[1];
            b.z = t->pos[2];
            b.w = 1.0f;
            func_001EFD20(0x8000002F, &b);
        }
        func_001EFD20(t->id, t->pos);
        t->done = 1;
        switch (func_00122BB8() % 3) {
        case 0:
            func_001F02C0(t->pos, 0x449, 200.0f);
            break;
        case 1:
            func_001F02C0(t->pos, 0x44A, 200.0f);
            break;
        case 2:
            func_001F02C0(t->pos, 0x44B, 200.0f);
            break;
        }
    }
}
