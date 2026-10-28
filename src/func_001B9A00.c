// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28. Split out of
// func_001B99F0 (splat had merged the two into one symbol); config/symbol_addrs.txt
// names it. The C is unchanged from the merged file's second function.
//
// Script opcode 0x0A PLAYER ANIM (see func_001BA1F0), an 8-way switch on the
// record's sub-command.

extern unsigned char D_008102B0[];      /* the player object */
extern void *D_0028A490[];              /* model/asset table */
extern void *D_0028A580;                /* default asset     */
extern volatile float D_00810350;
extern volatile int   D_00810354;
extern volatile float D_00810358;
extern volatile float D_00810360;
extern volatile float D_00810368;

extern void func_00102948(void *dst, void *src);        /* lq/sq quad copy */
extern void func_00182F90(void *player, void *point);
extern void func_001798D0(void *player);


/* opcode 0x0A PLAYER ANIM; its own function at 0x001B9A00 */
int func_001B9A00(void *actor, void *blk, unsigned char *rec)
{
    unsigned char *player = D_008102B0;

    switch (*(int *)(rec + 0x8)) {
    case 4:                                 /* bind asset + mode 3, then 0 */
        *(void **)(player + 0x40) = D_0028A490[*(int *)(rec + 0x1C)];
        player[0x2F3] = 3;
        /* fallthrough */
    case 0:                                 /* anim id + rate, +0x1F4 = 1.0f */
        *(short *)(player + 0x1F2) = *(short *)(rec + 0x14);
        *(float *)(player + 0x1F8) = *(float *)(rec + 0xC);
        *(int   *)(player + 0x1F4) = 0x3F800000;
        break;

    case 1:                                 /* bind asset + id + mode 1 */
        *(void **)(player + 0x40) = D_0028A490[*(int *)(rec + 0x1C)];
        *(short *)(player + 0x1F2) = *(short *)(rec + 0x14);
        player[0x2F3] = 1;
        *(float *)(player + 0x1F4) = *(float *)(rec + 0xC);
        *(int   *)(player + 0x200) = 0;
        break;

    case 2:                                 /* reset to the default asset */
        *(short *)(player + 0x1F2) = 0;
        *(short *)(player + 0x20C) = -1;
        player[0x2F3] = 3;
        *(void **)(player + 0x40) = D_0028A580;
        break;

    case 3:                                 /* wait for the anim to finish */
        return (*(int *)(player + 0x200) & 0x1000) ? 1 : 0;

    case 5:                                 /* move to rec+0x30, y (+0x34) -= 11 */
        func_00102948(rec + 0x30, *(unsigned char **)(player + 0x114) + 0xC0);
        *(float *)(rec + 0x34) -= 11.0f;
        func_00182F90(player, rec + 0x30);
        return 1;

    case 6:                                 /* camera-angle reset */
        D_00810350 = D_00810360;
        D_00810354 = 0;
        D_00810358 = D_00810368;
        break;

    case 7:                                 /* id / rate / blend from rec */
        *(short *)(player + 0x1F2) = *(short *)(rec + 0x14);
        *(float *)(player + 0x1F8) = *(float *)(rec + 0xC);
        *(float *)(player + 0x1F4) = *(float *)(rec + 0x10);
        break;

    case 8:                                 /* player re-init */
        func_001798D0(D_008102B0);
        break;
    }
    return 1;
}
