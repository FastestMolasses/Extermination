// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player effect emitter: after func_00182870(e, 0), surface type +0x23A 5 or
// 6 spawns effect 0x80000028 at the position +0xB0 (direction +0xC0); else a
// set +0x23C spawns 0x80000016 at the position with its y replaced by
// +0x250 (copy through func_001031E0); else, unless +0x23D is set, effect
// 0x80000011 at the position. func_001EFD90(kind, pos, dir) spawns.
extern void func_00182870(unsigned char *e, int f);
extern void func_001031E0(void *dst, void *src);
extern void func_001EFD90(int kind, void *pos, void *dir);

void func_0017DEB0(unsigned char *e) {
    float pos[4];

    func_00182870(e, 0);
    if (e[0x23A] == 6 || e[0x23A] == 5) {
        func_001EFD90(0x80000028, e + 0xB0, e + 0xC0);
        return;
    }
    if (e[0x23C] != 0) {
        func_001031E0(pos, e + 0xB0);
        pos[1] = *(float *)(e + 0x250);
        func_001EFD90(0x80000016, pos, e + 0xC0);
        return;
    }
    if (e[0x23D] == 0) {
        func_001EFD90(0x80000011, e + 0xB0, e + 0xC0);
    }
}
