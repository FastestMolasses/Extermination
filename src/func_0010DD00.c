// NEARMISS func_0010DD00  (vram 0x0010DD00, 0x134 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.95% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc coloring/list-scheduler wall. Body fully decoded and correct (DMA-chain/queue builder: validates (unsigned)(arg3-0x10)<0x61, builds a 16-byte-stride descriptor array on stack {ptr,D_00277220,size,0x44}, optional cache-flush via func_0010DFD8 when arg1&4, then _isceSifSetDma(arr,count) if arg1&1 else iGsPutIMR(...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void func_0010DFD8(void *addr, int size);
extern int _isceSifSetDma(void *p, int n);
extern int iGsPutIMR(void *p, int n);

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
        *(int *)((char *)arg2 + 4) = arg5;
        local[0].a = arg4;
        *(int *)arg2 = *(unsigned char *)arg2 | (arg6 << 8);
        local[0].b = arg5;
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
    local[count].d = 0x44;
    *(int *)((char *)arg2 + 8) = arg0;
    *(unsigned char *)arg2 = (unsigned char)arg3;

    func_0010DFD8(arg2, arg3);
    count++;

    if (arg1 & 1) {
        return _isceSifSetDma(local, count);
    }
    return iGsPutIMR(local, count);
}
