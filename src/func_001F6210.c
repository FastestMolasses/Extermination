// NEARMISS func_001F6210  (vram 0x001F6210, 0x424 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation / callee-save-count wall: the target's per-record body fits in 5 saved GPRs (s0-s4, all address-reused across the DMA-tag writes and the search loop) while this compile's regalloc keeps 2-3 extra stack-array base addresses (sp90/spA0/the search pointer) resident in s5-s7 inste...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// fx_render: iterates the func_001F5CA0() particle/fx record list, computes
// each record's world transform (func_001029C0/func_00102C58/func_00102918),
// resolves its sound-cue id via a linear search over D_0025CA40[] (idx*8
// entries, offset+4 = cue table row into D_0026EB20), applies a random blend
// jitter (func_00122BB8), then emits three DMA tag/packet blocks (types 5/9/0)
// into the actor ring at D_00275670->+0x1C before dispatching func_001D3D90 and
// func_001CAAC0. Logic/structure fully recovered; residual is register
// allocation (target uses 5 saved GPRs, this compile uses 7-8) -- a frame-size/
// callee-save-count scheduling wall in the large per-record body.
//
extern char *func_001F5CA0(void);
extern void func_001D8C20(int);
extern int func_001C6120(int, short);
extern void func_001029C0(void *out);
extern void func_00102C58(void *out, void *a, void *b);
extern void func_00102918(void *out, void *a, void *b);
extern void func_00102948(void *out, void *a);
extern int func_00122BB8(void);
extern void func_001028B8(void *out, void *a, void *b, int n);
extern void func_001026D0(void *out, void *a, void *b);
extern void func_001D3D90(int);
extern void func_001CAAC0(void *out, char *a, char *b);

typedef int s128 __attribute__((mode(TI)));

extern char *D_00275670;
extern int D_0028A59C;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern char D_0025CA40[];
extern char D_0026EB20[];
extern char D_0026EB60[];
extern int D_70003400[28];
extern char D_70003440[];
extern char D_70003470[];
extern char D_70003AC0[];

void func_001F6210(void) {
    char *rec = func_001F5CA0();
    int base = ((D_00810700 << 8) + D_00810701) << 8;
    if (rec == 0) {
        return;
    }
    func_001D8C20(1);
    while (*(short *)(rec + 0) >= 0) {
        char *owner = *(char **)(D_00275670 + 0x1C);
        int handle = func_001C6120(D_0028A59C, *(short *)(rec + 4));
        float sp80[4];
        float sp70[4];
        float spA0[4];
        float sp90[4];
        int key;
        char *p;
        unsigned int idx;

        sp80[0] = *(float *)(rec + 0x18);
        sp80[1] = *(float *)(rec + 0x1C);
        sp80[2] = *(float *)(rec + 0x20);
        *(int *)&sp80[3] = 0;
        sp70[0] = *(float *)(rec + 0xC);
        sp70[1] = *(float *)(rec + 0x10);
        sp70[2] = *(float *)(rec + 0x14);
        *(int *)&sp70[3] = 0x3F800000;

        func_001029C0(spA0);
        func_00102C58(spA0, spA0, sp80);
        func_00102918(spA0, spA0, sp70);

        key = *(short *)(rec + 4) + base;
        func_00102948(sp90, D_0026EB20);
        p = D_0025CA40;
        idx = 0;
        while (1) {
            if (key == *(int *)p) {
                func_00102948(sp90, D_0026EB20 + (*(short *)(p + 4) * 0x10));
                break;
            }
            idx += 1;
            p += 8;
            if (idx >= 0x11U) {
                break;
            }
        }

        {
            float az = sp90[3];
            float negaz = -az;
            float t = negaz + ((az - negaz) * (4.656613e-10f * (float) func_00122BB8()));
            sp90[0] += sp90[0] * t;
            sp90[1] += sp90[1] * t;
            sp90[2] += sp90[2] * t;
            sp90[3] = 0.0f;
        }

        *(unsigned char *)(*(char **)(D_00275670 + 0x1C) + 3) = 0x10;
        *(int *)(*(char **)(D_00275670 + 0x1C) + 4) = 0;
        *(short *)(*(char **)(D_00275670 + 0x1C) + 0) = 5;
        {
            char *base0 = *(char **)(D_00275670 + 0x1C);
            char *tag = base0 + 0x10;
            *(char **)(D_00275670 + 0x1C) = base0 + 0x60;

            D_70003400[0] = 0;
            D_70003400[1] = 0;
            D_70003400[2] = 0;
            D_70003400[3] = 0;
            D_70003400[4] = 0;
            D_70003400[5] = 0;
            D_70003400[6] = 0;
            D_70003400[7] = 0;
            D_70003400[8] = 0;
            D_70003400[9] = 0;
            D_70003400[10] = 0;
            D_70003400[11] = 0;
            D_70003400[12] = 0;
            D_70003400[13] = 0;
            D_70003400[14] = 0;
            D_70003400[15] = 0;
            D_70003400[16] = 0;
            D_70003400[17] = 0;
            D_70003400[18] = 0;
            D_70003400[19] = 0;
            D_70003400[20] = 0;
            D_70003400[21] = 0;
            D_70003400[22] = 0;
            D_70003400[23] = 0;
            D_70003400[24] = 0;
            D_70003400[25] = 0;
            D_70003400[26] = 0;
            D_70003400[27] = 0;

            func_001028B8(D_70003470, sp90, D_0026EB60, 0x10);

            *(int *)(base0 + 0x10) = 0x11000000;
            *(int *)(tag + 4) = 0x01000101;
            *(int *)(tag + 8) = 0;
            *(int *)(tag + 0xC) = 0x6C0403F5;
            ((s128 *)(tag + 0x10))[0] = ((s128 *)D_70003440)[0];
            ((s128 *)(tag + 0x20))[0] = ((s128 *)D_70003440)[1];
            ((s128 *)(tag + 0x30))[0] = ((s128 *)D_70003440)[2];
            ((s128 *)(tag + 0x40))[0] = ((s128 *)D_70003440)[3];
        }

        *(unsigned char *)(*(char **)(D_00275670 + 0x1C) + 3) = 0x10;
        *(int *)(*(char **)(D_00275670 + 0x1C) + 4) = 0;
        *(short *)(*(char **)(D_00275670 + 0x1C) + 0) = 9;
        {
            char *tag2 = *(char **)(D_00275670 + 0x1C);
            *(char **)(D_00275670 + 0x1C) = tag2 + 0xA0;
            *(s128 *)(tag2 + 0x10) = 0;
            *(int *)(tag2 + 0x14) = 0x01000101;
            *(int *)(tag2 + 0x18) = 0;
            *(int *)(tag2 + 0x1C) = 0x6C080000;
            func_001026D0(tag2 + 0x10 + 0x10, D_70003AC0, spA0);
            func_001026D0(tag2 + 0x10 + 0x50, D_70003400, spA0);
        }

        func_001D3D90(handle);
        *(unsigned char *)(*(char **)(D_00275670 + 0x1C) + 3) = 0x60;
        *(int *)(*(char **)(D_00275670 + 0x1C) + 4) = 0;
        *(short *)(*(char **)(D_00275670 + 0x1C) + 0) = 0;
        *(char **)(D_00275670 + 0x1C) = *(char **)(D_00275670 + 0x1C) + 0x10;

        func_001CAAC0(sp70, owner, D_00275670);
        rec += 0x28;
    }
    func_001D8C20(0);
}
