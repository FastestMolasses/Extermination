// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SEMANTICS (s15 decode): area-load actor SPAWNER. desc = D_0024D7C0[D_00810700];
// table = desc[D_00810701]; for each 0x28-byte placement record until cls==0xFF:
// skip cls&0xFF==0x0B (scripted/deferred); else actor = func_001AFA90(cls&0xFF).
// If the pool is empty (returns 0) the record is SKIPPED and the scan CONTINUES
// (idx still advances) — NOT an abort. Copy: model->+3, flags2 (lh rec+2 >>8 &0xFF)
// ->+0x2E(u16), param lo byte->+0xD, table index->+0x9A, kind->+0x54, link->+0x56,
// pos->+0xB0/B4/B8, rot->+0xC0/C4/C8, behavior fn->+0x10; uid->+0xE(u16) EXCEPT
// class 2 (rec[0]&0xFFFFFF1F==2, link-special) which instead gets +0x9D=D_00810701
// and +0x9E=uid lo byte.
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_0024D7C0[];
extern void func_001B6910(void);
extern void *func_001AFA90(int cls);

void func_001B6990(void) {
    unsigned char *rec;
    unsigned char *actor;
    int *table;
    int idx;
    int cls;

    func_001B6910();
    table = (int *)D_0024D7C0[D_00810700];
    if (table == 0) {
        return;
    }
    rec = (unsigned char *)((int *)table)[D_00810701];
    idx = 0;
    while ((cls = *(short *)(rec + 0x0)) != 0xFF) {
        if ((cls & 0xFF) != 0xB) {
            actor = (unsigned char *)func_001AFA90(cls & 0xFF);
            if (actor != 0) {
            *(actor + 0x3) = *(rec + 0x2);
            *(short *)(actor + 0x2E) = (*(short *)(rec + 0x2) >> 8) & 0xFF;
            *(actor + 0xD) = *(rec + 0x4);
            *(actor + 0x9A) = idx;
            if ((*(short *)(rec + 0x0) & -0xE1) == 2) {
                *(actor + 0x9D) = D_00810701;
                *(actor + 0x9E) = *(rec + 0x6);
            } else {
                *(unsigned short *)(actor + 0xE) = *(unsigned short *)(rec + 0x6);
            }
            *(short *)(actor + 0x54) = *(short *)(rec + 0x8);
            *(short *)(actor + 0x56) = *(short *)(rec + 0xA);
            *(float *)(actor + 0xB0) = *(float *)(rec + 0xC);
            *(float *)(actor + 0xB4) = *(float *)(rec + 0x10);
            *(float *)(actor + 0xB8) = *(float *)(rec + 0x14);
            *(float *)(actor + 0xC0) = *(float *)(rec + 0x18);
            *(float *)(actor + 0xC4) = *(float *)(rec + 0x1C);
            *(float *)(actor + 0xC8) = *(float *)(rec + 0x20);
            *(int *)(actor + 0x10) = *(int *)(rec + 0x24);
            }
        }
        rec = rec + 0x28;
        idx = idx + 1;
    }
}
