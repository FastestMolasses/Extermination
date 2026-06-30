// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// 4-state animation/timer state machine on arg0 (entity, state byte +6); arg1 is
// the work struct. case 0: advance state, init fields, zero gp-table[+4]+0x74,
// run anim_clip_init unless the masked +0x2C flags equal 9, and on +0x7B run
// func_0021C040 + clear the D_008106BD gate. case 1: when +0x3C<=26.0 or
// (+0x48<=0 && +0x74&2) advance state and clear +0x40. case 2: when (+0x48<=0 &&
// +0x74&2) advance, set +0x40/clear +0x48/+0x4C, fire func_001FBD50. case 3: on
// +0x30 & 0x1000 clear +5/+6.
// Idioms: D_008102B0/D_008106BD declared [] (far-abs, stay absolute) while gp-rel
// D_00275B40 stays gp-rel under -sdatathreshold 8; case-1 increment read through
// (volatile unsigned char*) to force the target's fresh lbu reload + nop slot
// (defeats mwcc CSE of the entry-load that would otherwise fill the bc1t slot).
extern void anim_clip_init(char *, int, short, float, float);
extern void func_001FBD50(char *, int, int, float);
extern void func_0021C040(void *, char *);
extern char *D_00275B40;
extern int D_008102B0[];
extern unsigned char D_008106BD[];

void func_00143610(char *arg0, char *arg1) {
    unsigned char st;
    short flags;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg1 + 0x75) = 0;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(*(char **)(D_00275B40 + 4) + 0x74) = 0;
        flags = *(short *)(arg0 + 0x2C);
        if ((flags & 0xFFFF7FFF) != 9) {
            anim_clip_init(arg0, 9, flags, 1.0f, 6.0f);
        }
        if (*(char *)(arg1 + 0x7B) != 0) {
            *(char *)(arg1 + 0x7B) = 0;
            func_0021C040(D_008102B0, arg0);
            if (D_008106BD[0] == 1) {
                D_008106BD[0] = 0;
            }
        }
        break;
    case 1:
        if (*(float *)(arg0 + 0x3C) <= 26.0f ||
            (*(float *)(arg1 + 0x48) <= 0.0f && (*(char *)(arg1 + 0x74) & 2))) {
            *(unsigned char *)(arg0 + 6) = *(volatile unsigned char *)(arg0 + 6) + 1;
            *(int *)(arg1 + 0x40) = 0;
        }
        break;
    case 2:
        if (*(float *)(arg1 + 0x48) <= 0.0f && (*(char *)(arg1 + 0x74) & 2)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(int *)(arg1 + 0x40) = 0x3F800000;
            *(float *)(arg1 + 0x48) = 0.0f;
            *(int *)(arg1 + 0x4C) = 0;
            func_001FBD50(arg0, 0x832, 0, 300.0f);
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
        }
        break;
    }
}
