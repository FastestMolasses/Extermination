// NEARMISS func_001CC8A0  (vram 0x001CC8A0, 0x254 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 77.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc-build ABI/register-model mismatch (NOT clean-store nop, NOT fixable by C reshaping). Target passes func_001CCE80's 5th arg on the stack (sd a2,0(sp); frame 0x40) and allocates temps in t0-t5/s0-s1 while homing params a1,a2 into t0,t1 at entry. Both available builds (991202 and 2.3.3) instead...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 77.56% (mwcc233; 991202=74.9%). Body/logic fully recovered.
// WALL: mwcc-build ABI/register-model mismatch, not a C-shape issue.
//   - Target passes the 5th arg to func_001CCE80 on the STACK ('sd a2,0(sp)',
//     frame 0x40); both available mwcc builds pass it in register a4 (frame 0x30).
//   - Target allocates temporaries in t0-t5/s0-s1 and homes params a1,a2 into
//     t0,t1 at entry; the available builds allocate a4-a7 as temps and skip the
//     param homing. This is the CW 2.3.1.01 arg-passing/reg model, unreachable
//     from 991202 or 2.3.3. Everything else (dispatch order, header stores,
//     packet call, both nibble-expansion loops, counts 0x20/10, dst strides
//     4/0x20, table D_0026E350 lookups) is byte-shape correct.
//
// SEMANTICS: builds a GIF/DMA color-LUT upload packet into the per-frame render
// context (*D_00275670)+0x14 write cursor. mode selects palette size:
//   mode==0: 16-entry LUT  (hdr qwc 0xF, cursor += 0x100), src table D_0028A494,
//            32 iterations, one source byte -> 2 halfwords (low/high nibble).
//   mode==1: 40-entry LUT  (hdr qwc 0x27, cursor += 0x280), src table D_0028A490,
//            10 iterations x 3 source bytes -> 6 halfwords each.
// Each nibble indexes the shared 16-entry halfword LUT D_0026E350. func_001CCE80
// writes the GIF tag / register descriptors (arg5 = per-mode qwc-ish 8 / 0x20).
// 'd' is a byte offset into the selected source table (per-model palette base).
extern char *D_00275670;
extern signed char D_0028A494[];
extern signed char D_0028A490[];
extern unsigned short D_0026E350[];
extern void func_001CCE80(int a, int b, int c, int d, long long e);

void func_001CC8A0(int mode, int b, int c, int d)
{
    signed char *p;
    unsigned short *dst;
    int i;
    int v;
    char *ctx;

    if (mode != 1) {
        if (mode == 0) {
            ctx = D_00275670;
            *(char *)(*(int *)(ctx + 0x14) + 3) = 0x10;
            *(int *)(*(int *)(ctx + 0x14) + 4) = 0;
            *(short *)(*(int *)(ctx + 0x14) + 0) = 0xF;
            dst = (unsigned short *)(*(int *)(ctx + 0x14) + 0x10);
            *(int *)(ctx + 0x14) = *(int *)(ctx + 0x14) + 0x100;
            func_001CCE80((int)dst, 0x1B00, 8, 0x14, 8);

            p = D_0028A494 + d;
            dst = (unsigned short *)((char *)dst + 0x70);
            for (i = 0; i < 0x20; i++) {
                v = p[0];
                dst[0] = D_0026E350[v & 0xF];
                dst[1] = D_0026E350[(v >> 4) & 0xF];
                p += 1;
                dst = (unsigned short *)((char *)dst + 4);
            }
        }
        return;
    }

    ctx = D_00275670;
    *(char *)(*(int *)(ctx + 0x14) + 3) = 0x10;
    *(int *)(*(int *)(ctx + 0x14) + 4) = 0;
    *(short *)(*(int *)(ctx + 0x14) + 0) = 0x27;
    dst = (unsigned short *)(*(int *)(ctx + 0x14) + 0x10);
    *(int *)(ctx + 0x14) = *(int *)(ctx + 0x14) + 0x280;
    func_001CCE80((int)dst, 0x1B00, 8, 0x14, 0x20);

    p = D_0028A490 + d;
    dst = (unsigned short *)((char *)dst + 0x70);
    for (i = 0; i < 10; i++) {
        v = p[0];
        dst[0] = D_0026E350[v & 0xF];
        dst[1] = D_0026E350[(v >> 4) & 0xF];
        v = p[1];
        dst[2] = D_0026E350[v & 0xF];
        dst[8] = D_0026E350[(v >> 4) & 0xF];
        v = p[2];
        dst[9] = D_0026E350[v & 0xF];
        dst[10] = D_0026E350[(v >> 4) & 0xF];
        p += 3;
        dst = (unsigned short *)((char *)dst + 0x20);
    }
}
