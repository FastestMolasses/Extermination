// NEARMISS func_0010E4C0  (vram 0x0010E4C0, 0x134 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 56.21% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Body byte-identical at correct 0x90 frame, but two deterministic ee-gcc 2.9 codegen selections diverge: (1) the null check `if(obj==0)return -1;` compiles to a forward branch-likely `bnezl s0` (delay-slot store) whereas expected uses plain `beqz s0` with the -1 in the delay slot (forward-branch-likely emission wall,...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void *func_0010E270(void *p);
extern int EndOfHeap(void *p);
extern int func_0010DE38(unsigned int a, void *b, int c, int d, int e, int f);
extern void SignalSema(int s);
extern void RFU063(int s);

extern unsigned char D_00278BC0;

int func_0010E4C0(int *out, int a1, int a2, int a3, int flags)
{
    int *obj;
    int local[5];
    int r;

    obj = (int *)func_0010E270(&D_00278BC0);
    if (obj == 0) {
        return -1;
    }

    out[0] = (int)obj;
    out[1] = obj[6];          /* obj->0x18 */
    obj[8] = a1;              /* 0x20 */
    obj[9] = a2;              /* 0x24 */
    obj[10] = a3;             /* 0x28 */
    obj[5] = (int)obj;        /* 0x14 */
    obj[7] = (int)out;        /* 0x1C */

    if (flags & 1) {
        out[2] = -1;          /* 0x8 */
        if (func_0010DE38(0x8000000C, obj, 0x40, 0, 0, 0) != 0) {
            return 0;
        }
        return -2;
    }

    local[1] = 1;
    local[2] = 0;
    r = EndOfHeap(&local[0]);
    out[2] = r;               /* sw v0,0x8(s1) in delay slot */
    if (r < 0) {
        return -3;
    }
    if (func_0010DE38(0x8000000C, obj, 0x40, 0, 0, 0) == 0) {
        return -2;
    }
    SignalSema(out[2]);
    RFU063(out[2]);
    return 0;
}
