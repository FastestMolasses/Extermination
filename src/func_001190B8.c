// COMPILER: eegcc
// CFLAGS: -O2

/*
 * Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 76.26% NEARMISS).
 * Fixes read from the original instructions:
 * - The voice table and the tempo/state block are typed records (Voice[48],
 *   TempoBlock) addressed directly, as in the byte-matched sound siblings
 *   (func_001181B0): one cached high half for D_0027F740 and gcc's own reversed
 *   loop counter instead of a hand-made element pointer.
 * - The three state words at D_0027F740 + 0x40/0x44/0x48 are volatile (shared with
 *   func_001152B0 / func_001157F0, which read them as volatile words); this keeps
 *   their stores in source order around the epilogue.
 * - func_001191F0 is declared with its src/ definition's six int parameters.
 *
 * Sound-system reset: posts request 0x65 through func_001191F0 with a 64-byte
 * parameter block {0x1E, arg0, .., 0} as both send and receive buffer; clears the
 * voice table (D_0027CCC0, 0x13E0 bytes), D_0027C6C0, D_0027E0C0, the state block
 * D_0027F740, D_0027F7C0, D_002817C0 and D_002819C0 with func_00121A28; sets the
 * tempo D_0027F740.f3A to 60; marks all 48 voices with f06 = 0xFFFF and f4E = 120;
 * then clears f48, sets f44 to 1 and clears f40.
 */

typedef struct {
    char pad_00[0x06];
    unsigned short f06;      /* 0x06 */
    char pad_08[0x46];
    unsigned short f4E;      /* 0x4E */
    char pad_50[0x1A];
} Voice;                     /* 0x6A */

typedef struct {
    char pad_00[0x3A];
    unsigned short f3A;      /* 0x3A */
    char pad_3C[0x04];
    volatile int f40;                 /* 0x40 */
    volatile int f44;                 /* 0x44 */
    volatile int f48;                 /* 0x48 */
    char pad_4C[0x04];
} TempoBlock;

extern int func_001191F0(int a0, int a1, int a2, int a3, int a4, int a5);
extern void func_00121A28(void *dst, int val, int size);

extern Voice D_0027CCC0[48];
extern TempoBlock D_0027F740;
extern unsigned char D_0027C6C0[];
extern unsigned char D_0027E0C0[];
extern unsigned char D_0027F7C0[];
extern unsigned char D_002817C0[];
extern unsigned char D_002819C0[];
void func_001190B8(int arg0)
{
    int local[16];
    int i;

    local[0] = 0x1E;
    local[1] = arg0;
    local[4] = 0;
    func_001191F0(0x65, 0, (int)local, (int)local, 0x40, 0x40);

    func_00121A28(D_0027CCC0, 0, 0x13E0);
    func_00121A28(D_0027C6C0, 0, 0x600);
    func_00121A28(D_0027E0C0, 0, 0x1680);
    func_00121A28(&D_0027F740, 0, 0x50);
    func_00121A28(D_0027F7C0, 0, 0x2000);
    func_00121A28(D_002817C0, 0, 0x200);
    func_00121A28(D_002819C0, 0, 0x100);

    D_0027F740.f3A = 0x3C;
    for (i = 0; i < 48; i++) {
        D_0027CCC0[i].f06 = 0xFFFF;
        D_0027CCC0[i].f4E = 0x78;
    }
    D_0027F740.f48 = 0;
    D_0027F740.f44 = 1;
    D_0027F740.f40 = 0;
}
