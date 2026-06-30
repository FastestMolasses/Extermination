// NEARMISS func_00110368  (vram 0x00110368, 0x130 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 71.57% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc global-address %hi-caching / coloring wall (+ loop branch-likely shape). Body fully decoded and correct (CpuConfig(); r=RFU116(0x80000000); D_00279680+0x14=arg1; strcpy arg0 -> D_00279680+0x18 counting len; set +0x4=0,+0x10=len,+0x8=0x80000003, mask qword &0xFFFFFFFF000000FF, byte0=0x68; build stack {base,r,0x...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int CpuConfig(void);
extern int RFU116(int a);
extern void func_0010DFD8(void *addr, int size);
extern int iGsPutIMR(void *p, int n);
extern int SetVSyncFlag(int a, int b);

extern unsigned char D_00279680[];

int func_00110368(char *arg0, int arg1)
{
    int local[4];
    unsigned char *base;
    int count;
    int r;

    CpuConfig();
    r = RFU116(0x80000000);

    base = D_00279680;
    *(int *)(base + 0x14) = arg1;

    count = 0;
    if (arg0[count] != 0) {
        do {
            base[0x18 + count] = (unsigned char)arg0[count];
            count++;
        } while (arg0[count] != 0);
    }

    *(int *)(base + 0x4) = 0;
    *(int *)(base + 0x10) = count;
    *(int *)(base + 0x8) = 0x80000003;
    *(long *)base = *(long *)base & 0xFFFFFFFF000000FFL;
    base[0] = 0x68;

    local[0] = (int)base;
    local[1] = r;
    local[2] = 0x68;
    local[3] = 0x44;
    func_0010DFD8(base, 0x68);

    if (iGsPutIMR(local, 1) != 0) {
        SetVSyncFlag(4, 0x10000);
        SetVSyncFlag(4, 0x20000);
        SetVSyncFlag(0x80000002, 0);
        SetVSyncFlag(0x80000000, 0);
        return 1;
    }
    return 0;
}
