// NEARMISS func_001FB9F0  (vram 0x001FB9F0, 0x254 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/instruction-scheduling permutation across a large (0x254-byte) 6-way symmetric branch-merge function (sound-id -> record-table dispatch, documented at docs/FINDINGS.md ~line 940 as the sound-engine's core id resolver). Logic and structure are fully recovered and match the disa...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Sound-id dispatch: mask id, bin into fixed / area-tabled / high-fixed
// record tables, resolve bank slot via D_00281D50, and submit the note-on
// via func_00119EA0 + follow-up pan/gain setters.

extern int D_0025ECA0[];
extern int D_00261570[];
extern int *D_00264A70[];
extern int *D_00264B30[];
extern int D_00264AD0[];
extern int *D_00264B90[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_00281D50[];

extern int func_00119EA0(int handle, int scriptGroup, int scriptIdx);
extern void func_0011A270(int voice, int a1);
extern void func_0011A218(int voice, int a1, int a2);

int func_001FB9F0(int id, int a1, int a2, int a3) {
    int *rec;
    int voice;

    id &= 0x7FFF;

    if (id < 0x3E8) {
        rec = (int *)((char *)D_0025ECA0 + id * 4);
    } else if (id < 0x5DC) {
        int *p = (int *)((char *)D_00264A70 + D_00810700 * 4);
        int *t = (int *)*p;
        if (t == 0)
            return -1;
        t = (int *)((char *)t + D_00810701 * 4);
        t = (int *)*t;
        if (t == 0)
            return (id - 0x3E8), -1;
        {
            unsigned char remap = *((unsigned char *)t + (id - 0x3E8));
            if (remap == 0xFF)
                return -1;
            {
                int *b = (int *)((char *)D_00264B30 + D_00810700 * 4);
                int *bb = (int *)*b;
                if (bb == 0)
                    return -1;
                bb = (int *)((char *)bb + D_00810701 * 4);
                {
                    int *rr = (int *)*bb;
                    if (rr == 0)
                        return -1;
                    rec = (int *)((char *)rr + remap * 4);
                }
            }
        }
    } else if (id < 0x7D0) {
        rec = (int *)((char *)D_00261570 + (id - 0x5DC) * 4);
    } else if (id < 0x9C4) {
        int *p = (int *)((char *)D_00264AD0 + D_00810700 * 4);
        int *t = (int *)*p;
        if (t == 0)
            return -1;
        t = (int *)((char *)t + D_00810701 * 4);
        {
            int *tt = (int *)*t;
            if (tt == 0)
                return (id - 0x7D0), -1;
            {
                unsigned char remap = *((unsigned char *)tt + (id - 0x7D0));
                if (remap == 0xFF)
                    return -1;
                {
                    int *b = (int *)((char *)D_00264B90 + D_00810700 * 4);
                    int *bb = (int *)*b;
                    if (bb == 0)
                        return -1;
                    bb = (int *)((char *)bb + D_00810701 * 4);
                    {
                        int *rr = (int *)*bb;
                        if (rr == 0)
                            return -1;
                        rec = (int *)((char *)rr + remap * 4);
                    }
                }
            }
        }
    } else {
        return -1;
    }

    {
        signed char *r8 = (signed char *)rec;
        int group = r8[0];
        int scriptGroup = r8[2];
        int bankIdx = r8[1];
        int handle = *(int *)((char *)D_00281D50 + (group * 0x14 + bankIdx) * 4);
        int scriptIdx = r8[3];

        voice = func_00119EA0(handle, scriptGroup, scriptIdx);
        if (voice < 0)
            return voice;
        func_0011A270(voice, a1);
        func_0011A218(voice, a2, a3);
        return voice;
    }
}
