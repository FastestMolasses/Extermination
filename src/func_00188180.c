// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-object state-machine step keyed on the byte at +5:
//   state 0: if the +0x2E flag bit 0x1000 is set, advance to state 1.
//   state 1: if gp-rel global (&D_00275B00)[2] is nonzero, go to state 2 and
//            kick off anim_clip_init(obj, 0xF, 0.0, 0.0).
//   state 2: if the +0x2E flag bit 0x1000 is set, set the +4 byte to 3.
// Then unconditionally copy the 4-qword block at D_00810380 into obj+0xD0 and
// run func_001C9940(obj+0x110, obj[0xC], obj+0xD0).
//
// Matched with mwcc 2.3.3; the pinned 991202 build reaches only 91.8% (switch
// dispatch / branch lowering). objdiff 100.0% vs expected.
extern void anim_clip_init(void *p, int n, float a, float b);
extern void copy_qw4(char *dst, char *src);
extern void func_001C9940(char *dst, int type, char *src);
extern int D_00275B00;
extern char D_00810380[];

void func_00188180(char *p) {
    unsigned char state;

    state = p[5];
    switch (state) {
    case 0:
        if (*(unsigned short *)(p + 0x2E) & 0x1000) {
            p[5] = state + 1;
        }
        break;
    case 1:
        if ((&D_00275B00)[2] != 0) {
            p[5] = 2;
            anim_clip_init(p, 0xF, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (*(unsigned short *)(p + 0x2E) & 0x1000) {
            p[4] = 3;
        }
        break;
    }
    copy_qw4(p + 0xD0, D_00810380);
    func_001C9940(p + 0x110, (unsigned char)p[0xC], p + 0xD0);
}
