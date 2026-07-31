// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Registers a dynamic point light in the global pool D_00275670: if the live count at +0x214 is
// already 0x20 returns -1, else carves slot = base + (count<<7) + 0x1220, copies the position and
// colour records into slot+0x10/slot+0x20, stores the two float params at slot+0/+4, takes the slot
// id from the post-incremented allocator at +0x210, stores the type at slot+8, bumps the live count
// and returns the id.

extern char *D_00275670;
extern void func_00102948(void *dst, void *src);

int func_001D7FA0(void *pos, void *color, float fa, float fb, int type) {
    char *slot;
    int idx;

    idx = *((int *) (D_00275670 + 0x214));
    if (idx >= 0x20) {
        return -1;
    }
    slot = D_00275670 + (idx << 7) + 0x1220;
    func_00102948(slot + 0x10, pos);
    func_00102948(slot + 0x20, color);
    *((float *) (slot + 0x0)) = fa;
    *((float *) (slot + 0x4)) = fb;
    *((int *) (slot + 0xC)) = (*((int *) (D_00275670 + 0x210)))++;
    *((int *) (slot + 0x8)) = type;
    (*((int *) (D_00275670 + 0x214)))++;
    return *((int *) (slot + 0xC));
}
