// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 77.56% NEARMISS).
// Fixes read from the original instructions:
//  - func_001CCE80 takes 9 arguments (its definition in src/): the caller passes
//    its own b and c through as dsax/dsay plus rrw/rrh and qwc; the 9th goes on the
//    stack. The old 5-argument extern was the "ABI/register-model wall".
//  - D_0028A490/D_0028A494 are entries of the per-model pointer table (the code
//    loads a pointer, not the address of an array); mode 0 reads entry 1.
//  - the dispatch is switch (mode) { case 0 ... case 1 ... } with break;
//  - each source byte is read again for its high nibble (no temp);
//  - the loop counter is declared first, and the table pointer is loaded then
//    offset by d as two statements.
//
// SEMANTICS: builds a GIF/DMA color-LUT upload packet into the per-frame render
// context (*D_00275670)+0x14 write cursor. mode selects palette size:
//   mode==0: 16-entry LUT  (hdr qwc 0xF, cursor += 0x100), source pointer
//            D_0028A494, 32 iterations, one source byte -> 2 halfwords (low/high nibble).
//   mode==1: 40-entry LUT  (hdr qwc 0x27, cursor += 0x280), source pointer
//            D_0028A490, 10 iterations x 3 source bytes -> 6 halfwords each.
// Each nibble indexes the shared 16-entry halfword LUT D_0026E350. func_001CCE80
// writes the image-transfer header (BITBLTBUF/TRXPOS/TRXREG, rect b,c sized
// 16x16 or 32x32, qwc 8 / 0x20). 'd' is a byte offset into the selected source.
extern char *D_00275670;
extern signed char *D_0028A494[];
extern signed char *D_0028A490[];
extern unsigned short D_0026E350[];
extern void func_001CCE80(char *pkt, int dbp, int dbw, int dpsm, int dsax, int dsay,
                          int rrw, int rrh, int qwc);

void func_001CC8A0(int mode, int b, int c, int d)
{
    int i;
    signed char *p;
    unsigned short *dst;
    char *ctx;

    switch (mode) {
    case 0:
        ctx = D_00275670;
        *(char *)(*(int *)(ctx + 0x14) + 3) = 0x10;
        *(int *)(*(int *)(ctx + 0x14) + 4) = 0;
        *(short *)(*(int *)(ctx + 0x14) + 0) = 0xF;
        dst = (unsigned short *)(*(int *)(ctx + 0x14) + 0x10);
        *(int *)(ctx + 0x14) = *(int *)(ctx + 0x14) + 0x100;
        func_001CCE80((char *)dst, 0x1B00, 8, 0x14, b, c, 0x10, 0x10, 8);

        p = D_0028A494[0];
        p += d;
        dst = (unsigned short *)((char *)dst + 0x70);
        for (i = 0; i < 0x20; i++) {
            dst[0] = D_0026E350[p[0] & 0xF];
            dst[1] = D_0026E350[(p[0] >> 4) & 0xF];
            p += 1;
            dst = (unsigned short *)((char *)dst + 4);
        }
        break;
    case 1:
        ctx = D_00275670;
        *(char *)(*(int *)(ctx + 0x14) + 3) = 0x10;
        *(int *)(*(int *)(ctx + 0x14) + 4) = 0;
        *(short *)(*(int *)(ctx + 0x14) + 0) = 0x27;
        dst = (unsigned short *)(*(int *)(ctx + 0x14) + 0x10);
        *(int *)(ctx + 0x14) = *(int *)(ctx + 0x14) + 0x280;
        func_001CCE80((char *)dst, 0x1B00, 8, 0x14, b, c, 0x20, 0x20, 0x20);

        p = D_0028A490[0];
        p += d;
        dst = (unsigned short *)((char *)dst + 0x70);
        for (i = 0; i < 10; i++) {
            dst[0] = D_0026E350[p[0] & 0xF];
            dst[1] = D_0026E350[(p[0] >> 4) & 0xF];
            dst[2] = D_0026E350[p[1] & 0xF];
            dst[8] = D_0026E350[(p[1] >> 4) & 0xF];
            dst[9] = D_0026E350[p[2] & 0xF];
            dst[10] = D_0026E350[(p[2] >> 4) & 0xF];
            p += 3;
            dst = (unsigned short *)((char *)dst + 0x20);
        }
        break;
    }
}
