// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Camera/effect shake-decay step. Builds two scratchpad vectors
// (D_700038A0, D_700038B0) from the actor at arg0->0x114 (+0xC0 sub-object)
// via func_00102948, decays the shake magnitude at 0x700038B4 by 20.0f, and
// if func_0019A570(...,4,0) reports the region is still active, refreshes the
// vectors from the config object (*0x700031D0)+0x24..0x2C and re-applies the
// transform via func_001F9100(actor+0xC0, vecs, 5.0f).

extern void func_00102948(void *dst, void *src);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001F9100(int handle, void *a, void *b, float t);

extern int D_700038A0;
extern int D_700038B0;
extern int D_700031B0;

void func_001BA7F0(unsigned char *arg0) {
    int *src;

    func_00102948(&D_700038A0, (char *)*(int *)(arg0 + 0x114) + 0xC0);
    func_00102948(&D_700038B0, &D_700038A0);
    *(float *)0x700038B4 = *(float *)0x700038B4 - 20.0f;
    if (func_0019A570(&D_700038A0, &D_700038B0, 4, 0) != 0) {
        func_00102948(&D_700038A0, &D_700031B0);
        src = *(int **)0x700031D0;
        *(float *)0x700038B0 = *(float *)((unsigned char *)src + 0x24);
        *(float *)0x700038B4 = *(float *)((unsigned char *)src + 0x28);
        *(float *)0x700038B8 = *(float *)((unsigned char *)src + 0x2C);
        func_001F9100(*(int *)(arg0 + 0x114) + 0xC0, &D_700038A0, &D_700038B0, 5.0f);
    }
}
