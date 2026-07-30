// NEARMISS func_001B8020  (vram 0x001B8020, 0x1A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.86% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 2 of 106 instructions, case 4 only (float-arg register assignment, NOT a dispatch issue — jr-table + all 7 case bodies are byte-identical). For anim_clip_init(self, clip, 0.0f, 0.0f) CW materializes the shared zero into the SECOND float arg reg and copies down: `mtc1 zero,$f13` ... `mov.s $f12,$f...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: animation-clip state-machine step. Dispatches on the mode word at
// cfg+8 through the 7-entry jr-table at 0x0026DFC0. Returns 1 when the step is
// complete / should advance, 0 while it is still running.
//   6: func_001FBD50(self, cfg+0x18, 0, 300.0f), then FALL THROUGH into 0
//   0: anim_clip_init(self, (short)cfg+0x14, cfg+0xC, 0.0f); clear st+0xE; return 1
//   1: return (st+0xE & 0x1000) != 0
//   2: st+0xE = anim_advance_time(self, cfg+0xC);
//      return (st+0xE & (int)cfg+0x14) != 0
//   3: st+0xE = anim_advance_time(self, cfg+0xC); then on the sub-phase byte st+4:
//        0 -> cfg+0x10 = (float)(int)cfg+0x14, st+4++ and fall into 1
//        1 -> cfg+0x10 -= 1.0f; return (cfg+0x10 == 0.0f)
//        anything else -> return 1
//   4: self+0x40 = D_0028A490[cfg+0x1C];
//      anim_clip_init(self, (short)cfg+0x14, 0.0f, 0.0f); clear st+0xE; return 1
//   5: self+0x40 = D_0028A490[cfg+0x1C];
//      bone_init_default_2(self, (short)cfg+0x14); clear st+0xE; return 1
//   mode >= 7: return 1

extern short anim_advance_time(char *p, float dt);
extern void anim_clip_init(char *p, int clip, float a, float b);
extern void bone_init_default_2(char *p, int n);
extern void func_001FBD50(char *p, int id, int flag, float a);
extern int D_0028A490[];

int func_001B8020(char *self, char *st, char *cfg) {
    switch (*(int *)(cfg + 8)) {
    case 6:
        func_001FBD50(self, *(int *)(cfg + 0x18), 0, 300.0f);
        /* fallthrough */
    case 0:
        anim_clip_init(self, *(short *)(cfg + 0x14), *(float *)(cfg + 0xC), 0.0f);
        *(short *)(st + 0xE) = 0;
        break;
    case 1:
        return (*(short *)(st + 0xE) & 0x1000) ? 1 : 0;
    case 2:
        *(short *)(st + 0xE) = anim_advance_time(self, *(float *)(cfg + 0xC));
        return (*(short *)(st + 0xE) & *(int *)(cfg + 0x14)) ? 1 : 0;
    case 3:
        {
            float x;
            float zero = 0.0f;

            *(short *)(st + 0xE) = anim_advance_time(self, *(float *)(cfg + 0xC));
            switch (*(unsigned char *)(st + 4)) {
            case 0:
                *(float *)(cfg + 0x10) = (float)*(int *)(cfg + 0x14);
                *(unsigned char *)(st + 4) = *(unsigned char *)(st + 4) + 1;
                /* fallthrough */
            case 1:
                x = *(float *)(cfg + 0x10) - 1.0f;
                *(float *)(cfg + 0x10) = x;
                if (x == zero) {
                    return 1;
                }
                return 0;
            }
        }
        break;
    case 4:
        {
            float zero = 0.0f;

            *(int *)(self + 0x40) = D_0028A490[*(int *)(cfg + 0x1C)];
            anim_clip_init(self, *(short *)(cfg + 0x14), zero, zero);
            *(short *)(st + 0xE) = 0;
        }
        break;
    case 5:
        *(int *)(self + 0x40) = D_0028A490[*(int *)(cfg + 0x1C)];
        bone_init_default_2(self, *(short *)(cfg + 0x14));
        *(short *)(st + 0xE) = 0;
        break;
    }
    return 1;
}
