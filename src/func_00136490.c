// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame state advance for an animated entity (state = p[6]).
//   case 0: bump state; set *(float*)(a1+0x20)=1.0f; set global flag D_00810805=8.
//   case 1: when D_00810805==9, bump state, zero p+0xC4, seed p+0xB0=340.0f and
//           p+0xB8=270.0f, clear a1+0x30/a1+0x2C, start anim clip 0x1D (5.0f),
//           bind handler func_001EFE00(0x80000046,p)->p+0x24, then issue two
//           func_001FBD50 events (0x857, 0x858) at 450.0f.
//   case 2: when (a1[0] & 0x1000), bump state.
//   case 3: when D_0028A9A2 && D_0028A9A0==2, set p[4]=3 and, if p+0x24 is live,
//           write 3 to *(p+0x24)+4 and clear p+0x24.
// Returns void. Cases written in ASCENDING source order so mwcc tests them in
// DESCENDING order (3,2,1,0) to match CodeWarrior's beq dispatch chain.
extern void anim_clip_init(void *self, int clip, float f12, float f13);
extern int func_001EFE00(unsigned int a, void *p);
extern void func_001FBD50(void *self, int a1, int a2, float f12);
extern unsigned char D_00810805;
extern char D_0028A9A2;
extern short D_0028A9A0;

void func_00136490(unsigned char *p, int *a1) {
    int st = p[6];

    switch (st) {
    case 0:
        p[6] = st + 1;
        *(int *)(a1 + 0x20 / 4) = 0x3F800000;
        D_00810805 = 8;
        break;
    case 1:
        if (D_00810805 == 9) {
            p[6] = st + 1;
            *(int *)(p + 0xC4) = 0;
            *(int *)(p + 0xB0) = 0x43AA0000;
            *(int *)(p + 0xB8) = 0x43870000;
            a1[0x30 / 4] = 0;
            a1[0x2C / 4] = 0;
            anim_clip_init(p, 0x1D, 5.0f, 0.0f);
            *(int *)(p + 0x24) = func_001EFE00(0x80000046, p);
            func_001FBD50(p, 0x857, 0, 450.0f);
            func_001FBD50(p, 0x858, 0, 450.0f);
        }
        break;
    case 2:
        if ((a1[0] & 0x1000) != 0) {
            p[6] = st + 1;
        }
        break;
    case 3:
        if (D_0028A9A2 != 0 && D_0028A9A0 == 2) {
            p[4] = 3;
            if (*(int *)(p + 0x24) != 0) {
                *(char *)(*(int *)(p + 0x24) + 4) = 3;
                *(int *)(p + 0x24) = 0;
            }
        }
        break;
    }
}
