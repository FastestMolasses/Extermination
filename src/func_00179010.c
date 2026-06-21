// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Clean-store beqz delay-slot-nop wall: target+2.3.3 leave the beqz delay slot
// as a nop; the pinned 991202 build fills it (97.37%). Matched on mwcc 2.3.3.
// The D_700038A0 block stores go through literal absolute addresses (lui/sw),
// but the &D_700038A0 passed as func_0019AB20's 3rd arg must reference the
// symbol so mwcc emits lui%hi/addiu%lo (not lui/ori). Storing a0[0x23B] BEFORE
// the func_0019A310 call lets mwcc schedule that sb into the call's delay slot.
extern int func_0019AB20(int, int, void *, int);
extern void func_0019A310(void *);
extern unsigned char D_700038A0[];

int func_00179010(unsigned char *a0) {
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0xC0C00000;
    *(int *)0x700038A8 = 0;
    *(float *)0x700038AC = 1.0f;
    if (func_0019AB20((int)a0, (int)(a0 + 0xB0), D_700038A0, 0x80000006)) {
        a0[0xA] = 1;
        a0[0x23B] = *(unsigned char *)(*(int *)0x700031D0 + 0x1A);
        func_0019A310(a0 + 0x9C);
    } else {
        a0[0x23B] = 0;
    }
    return a0[0xA];
}
