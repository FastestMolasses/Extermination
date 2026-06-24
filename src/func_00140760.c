// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Two-state actor update dispatched on the state byte at arg0+6.
//   state 0 (spawn/intro): advance the state byte to 1, write 1.0f to arg1+0x40,
//     clear flags at arg1+0x81/0x50/0x4C, then pick an anim clip: if the entity's
//     flags halfword (arg0+0x36) has bit 0x8000 set, fire event 0x80F and play
//     clip 0x21; otherwise fire event 0x80E and play a clip chosen from the
//     gp-rel short table D_002753C0 indexed by (func_00122BB8() >> 3) % 3. Both
//     play at speed 1.0f. If the pending-flag byte arg1+0x89 is set, clear it,
//     run func_0021C040(&D_008102B0, arg0), and if global D_008106BD == 1 clear it.
//   state 1 (despawn check): if arg1+0x30 has bit 0x1000, reset the actor record
//     (bytes +4=1,+5=8,+6=0,+0=1; halfword +0x36=0; arg1 bytes +0x88/+0x8D=0;
//     halfword arg1+0x70=0xF0).
// func_001FBD50(entity, eventId, 0, durationSecs) matches the matched callers
// (e.g. func_00136490). The 0x80E-arm clip index reads the gp-rel table so it
// needs -sdatathreshold 8; D_008102B0/D_008106BD stay hi/lo (sized [16]).
//
// mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build caps at 83.4% (it
// arranges the state-0 epilogue / clean-store delay slots differently). Verified
// objdiff 100% vs build/expected/func_00140760.o.
extern void anim_clip_init(char *a, int b, float c, float d);
extern int func_00122BB8(void);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_0021C040(void *a, char *b);
extern short D_002753C0[1];
extern int D_008102B0[16];
extern unsigned char D_008106BD[16];

void func_00140760(char *arg0, char *arg1) {
    unsigned char v = *(unsigned char *)(arg0 + 6);

    switch (v) {
    case 0:
        *(unsigned char *)(arg0 + 6) = v + 1;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(char *)(arg1 + 0x81) = 0;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        if (*(short *)(arg0 + 0x36) & 0x8000) {
            func_001FBD50(arg0, 0x80F, 0, 300.0f);
            anim_clip_init(arg0, 0x21, 1.0f, 0.0f);
        } else {
            func_001FBD50(arg0, 0x80E, 0, 300.0f);
            anim_clip_init(arg0, D_002753C0[(func_00122BB8() >> 3) % 3], 1.0f, 0.0f);
        }
        if (*(char *)(arg1 + 0x89) != 0) {
            *(char *)(arg1 + 0x89) = 0;
            func_0021C040(D_008102B0, arg0);
            if (D_008106BD[0] == 1) {
                D_008106BD[0] = 0;
            }
        }
        break;
    case 1:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            *(char *)(arg0 + 5) = 8;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg1 + 0x88) = 0;
            *(char *)(arg1 + 0x8D) = 0;
            *(short *)(arg1 + 0x70) = 0xF0;
        }
        break;
    }
}
