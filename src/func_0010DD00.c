// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.95% NEARMISS).
// Fixes read from the original instructions:
//  - The descriptor's 0x44 tag is written after the packet header stores
//    (packet word 2 = arg0, low byte = arg3), not with the other descriptor fields.
//  - In the arg6 > 0 arm the descriptor fields and packet header are written
//    interleaved: descriptor word 0, packet word 0, descriptor word 1, packet word 1.
//
// DMA-chain/queue builder: validates (unsigned)(arg3-0x10)<0x61, builds a
// 16-byte-stride descriptor array on the stack {ptr, D_00277220, size, 0x44},
// optionally flushes the cache for the payload via func_0010DFD8 when arg1&4,
// then submits with isceSifSetDma(arr,count) if arg1&1 else sceSifSetDma.

extern void func_0010DFD8(void *addr, int size);
extern int isceSifSetDma(void *p, int n);
extern int sceSifSetDma(void *p, int n);

extern int D_00277220;

typedef struct {
    int a;
    int b;
    int c;
    int d;
} Entry;

int func_0010DD00(int arg0, int arg1, void *arg2, int arg3,
                  int arg4, int arg5, int arg6)
{
    Entry local[2];
    int count;

    if ((unsigned int)(arg3 - 0x10) >= 0x61) {
        return 0;
    }

    count = 0;
    if (arg6 > 0) {
        count = 1;
        local[0].a = arg4;
        *(int *)arg2 = *(unsigned char *)arg2 | (arg6 << 8);
        local[0].b = arg5;
        *(int *)((char *)arg2 + 4) = arg5;
        local[0].c = arg6;
        local[0].d = 0;
        if (arg1 & 4) {
            func_0010DFD8((void *)arg4, arg6);
        }
    } else {
        *(int *)((char *)arg2 + 4) = 0;
        *(int *)arg2 = *(unsigned char *)arg2;
    }

    local[count].a = (int)arg2;
    local[count].b = D_00277220;
    local[count].c = arg3;
    *(int *)((char *)arg2 + 8) = arg0;
    *(unsigned char *)arg2 = (unsigned char)arg3;
    local[count].d = 0x44;

    func_0010DFD8(arg2, arg3);
    count++;

    if (arg1 & 1) {
        return isceSifSetDma(local, count);
    }
    return sceSifSetDma(local, count);
}
