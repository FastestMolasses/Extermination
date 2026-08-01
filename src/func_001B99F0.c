// NEARMISS func_001B99F0  (vram 0x001B99F0, 0x1AC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 2.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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

/* opcode 0x09 CALL NATIVE: tail-jump to the record's own handler */
int func_001B99F0(void *actor, void *blk, unsigned int *rec)
{
    return ((int (*)(void *, void *, unsigned int *))rec[1])(actor, blk, rec);
}

/* opcode 0x0A PLAYER ANIM; entered at func_001B99F0 + 0x10 */
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

    case 5:                                 /* move to rec+0x30, z -= 11 */
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
