// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA01 overlay, runtime 0x00824D50 (splat/link name 00824D10; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x823CD0 (O3 owner); builds the actor matrix from
//  +0xC0..0xC8 and a model bone (table 0x829DC0), then fires event 0x80000009
//  once +0xD4 reaches 2.0.
// Covers the splat pieces 00824D10, 00824D50 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
extern char *D_008106C0[2];
extern char *D_00275B40;
extern int D_overlay_AREA01_00829DC0[];
extern float D_70003000[12];
extern float D_70003030[4];
extern float D_70003400[16];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern void func_001287F0(void *a0, void *a1, short a2, float f12);
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102BB0(void *dst, void *src, float a);
extern void func_00102A60(void *dst, void *src, float a);
extern void func_001031E0(void *dst, void *src);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_00102958(void *dst, void *src);
extern void func_001C69A0(unsigned char *self);
extern void func_0012DE90(unsigned char *ev);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001EFD90(int id, void *a, void *b);

void func_overlay_AREA01_00824D10(unsigned char *self, unsigned char *ev) {
    char *w = D_008106C0[0];
    int idx = ev[0xE2];
    int st = self[6];
    switch (st) {
    case 0:
        self[6] = st + 1;
        func_001287F0(self, ev, 0x19, 0.0f);
        *(short *)(ev + 0xD0) = 0xF0;
        *(float *)(ev + 0xD4) = 1.0f;
        break;
    case 1:
        func_001029C0(D_70003000);
        func_00102B08(D_70003000, D_70003000, *(float *)(self + 0xC0));
        func_00102BB0(D_70003000, D_70003000, *(float *)(self + 0xC4));
        func_00102A60(D_70003000, D_70003000, *(float *)(self + 0xC8));
        func_001031E0(D_70003030, ev + 0x30);
        /* the (char *) round trip on the index term orders the address add */
        func_001026D0(D_70003000, *(char **)((char *)(D_overlay_AREA01_00829DC0[idx] << 2) + (int)w + 0x110) + 0x90, D_70003000);
        func_001031E0(self + 0xB0, D_70003030);
        func_00102958(D_70003400, D_70003000);
        func_001C69A0(self);
        func_0012DE90(ev);
        if (!(*(float *)(ev + 0xD4) < 2.0f)) {
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038B0 = 0.0f;
            *(float *)0x700038B4 = 0.0f;
            *(float *)0x700038B8 = 0.0f;
            *(float *)0x700038A4 = 1.0f;
            *(float *)0x700038A8 = 1.0f;
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038BC = 1.0f;
            func_001026A0(D_700038A0, *(char **)(D_00275B40 + 0xC) + 0x90, D_700038A0);
            func_001FB9F0(0x1B2, 0x1000, 0x1000, 0x1000);
            func_001EFD90(0x80000009, D_700038A0, D_700038B0);
            self[4] = 3;
        }
        break;
    }
}
