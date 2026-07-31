// COMPILER: eegcc
// CFLAGS: -O2
// Per-slot update step: run each active channel's pair of callbacks, warn if
// the slot is flagged, then hand the slot off to the appropriate finisher.

typedef struct {
    unsigned char unk00[0x1C];
} Chan; /* 0x1C */

typedef struct {                 /* 0x140 */
    int f00;                     /* 0x00 */
    int f04;                     /* 0x04 */
    unsigned char pad08[0x28 - 0x08];
    void (*fnA[4])(Chan *);      /* 0x28 */
    void (*fnB[4])(Chan *);      /* 0x38 */
    Chan chA[4];                 /* 0x48 */
    Chan chB[4];                 /* 0xB8 */
    int f128;                    /* 0x128 */
    int f12C;                    /* 0x12C */
    int f130;                    /* 0x130 */
    int f134;                    /* 0x134 */
    int f138;                    /* 0x138 */
    int f13C;                    /* 0x13C */
} Slot;

extern Slot D_002418F8[];
extern int D_002412F0;
extern const char D_0026B368[];

extern void func_0010A3A8(const char *msg);
extern int func_001050E8(int a0, int a1);
extern int func_00105088(int a0, int a1, int a2);

void sub_intra_skip_MB(int idx)
{
    int i;

    if (D_002418F8[idx].f138 != 0) {
        for (i = 0; i < D_002418F8[idx].f12C; i++) {
            D_002418F8[idx].fnA[i](&D_002418F8[idx].chA[i]);
            D_002418F8[idx].fnB[i](&D_002418F8[idx].chB[i]);
        }
    }
    if (D_002418F8[idx].f130 != 0 && D_002418F8[idx].f13C != 0) {
        func_0010A3A8(D_0026B368);
    }
    if (D_002418F8[idx].f130 != 0) {
        func_001050E8(D_002418F8[idx].f128, D_002418F8[idx].f04);
        return;
    }
    if (D_002418F8[idx].f13C != 0) {
        func_001050E8(D_002418F8[idx].f128, D_002412F0);
        return;
    }
    func_00105088(D_002418F8[idx].f128, D_002412F0, D_002418F8[idx].f04);
}
