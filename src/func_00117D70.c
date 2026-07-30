// NEARMISS func_00117D70  (vram 0x00117D70, 0x178 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.82% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc gcse address-hoist wall — target emits `lui %hi(D_00281AC0)` once per if/else arm (2 luis, one pulled into the bne delay slot); our ee-gcc's gcse hoists the shared %hi to the common dominator (1 lui). Knock-on: missing 16-byte loop-head alignment nop, post-loop `addiu v0,t0,5/3` sunk into t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// NEARMISS 84.82 — eegcc gcse address-hoist wall. Logic and structure fully
// recovered: instruction-for-instruction the two loops, both compare chains,
// the delay-slot annulled `continue` edges, the CSE of the compared constant 1
// into the `sh ..,0x14(ch)` store and the signed `slt` pointer bound (base+0x13E0)
// all reproduce exactly. Residual: the target materialises `lui %hi(D_00281AC0)`
// SEPARATELY in each arm of the if/else (2 luis; reorg pulls the else arm's copy
// into the `bne` delay slot), while our ee-gcc's gcse hoists the shared %hi part
// to the common dominator (1 lui). That one missing instruction also shifts the
// 16-byte loop-head alignment padding (a `nop` before .L00117DB0), and drags the
// post-loop `addiu v0,t0,5` / `addiu v0,t0,3` into the loop preheader plus a
// v0/a1-vs-a2 GPR-colouring shift. Not source-controllable: tried per-branch
// pointer locals, distinct variables, assignment inside the loop body, the
// read-modify-write memory form of the tail store, and -O1 (62.11%).

extern unsigned char D_00281AC0[];
extern unsigned char D_0027CCC0[];

void func_00117D70(unsigned char *ev)
{
    unsigned char *ch;
    unsigned char *snd;
    int off;
    int val;
    int i;

    if (*(unsigned short *)(ev + 0x34) == 1) {
        off = *(int *)(ev + 8);
        snd = D_00281AC0;
        for (i = 0; i < 0x30; i++) {
            ch = D_0027CCC0 + i * 0x6A;
            if (*(unsigned short *)(ch + 0x1A) != 2)
                continue;
            if (*(unsigned short *)(ch + 0x3E) !=
                *(unsigned char *)(off + *(int *)(snd + 0x14) + 3))
                continue;
            if (*(unsigned short *)(ch + 2) !=
                *(unsigned char *)(off + *(int *)(snd + 0x14) + 4))
                continue;
            if (*(unsigned short *)(ch + 0x22) != *(unsigned short *)(ev + 0x24))
                continue;
            if (*(unsigned short *)(ch + 6) != *(int *)(ev + 0x18))
                continue;
            if (*(unsigned short *)(ch + 0) != 1)
                continue;
            val = *(unsigned char *)(off + *(int *)(snd + 0x14) + 2);
            *(unsigned short *)(ch + 0x14) = 1;
            *(unsigned short *)(ch + 0x16) = val;
        }
        *(int *)(ev + 8) = off + 5;
    } else {
        snd = D_00281AC0;
        *(unsigned char *)(*(int *)(snd + 0xC) + 9) =
            *(unsigned char *)(*(int *)(ev + 8) + *(int *)(snd + 0x14) + 2);
        off = *(int *)(ev + 8);
        for (i = 0; i < 0x30; i++) {
            ch = D_0027CCC0 + i * 0x6A;
            if (*(unsigned short *)(ch + 4) != (*(unsigned char *)ev & 0xF))
                continue;
            if (*(unsigned short *)(ch + 0x22) != *(unsigned short *)(ev + 0x24))
                continue;
            if (*(unsigned short *)(ch + 6) != *(int *)(ev + 0x18))
                continue;
            if (*(unsigned short *)(ch + 0x1A) != 1)
                continue;
            if (*(unsigned short *)(ch + 0) != 1)
                continue;
            val = *(unsigned char *)(off + *(int *)(snd + 0x14) + 2);
            *(unsigned short *)(ch + 0x14) = 1;
            *(unsigned short *)(ch + 0x16) = val;
        }
        *(int *)(ev + 8) = off + 3;
    }
}
