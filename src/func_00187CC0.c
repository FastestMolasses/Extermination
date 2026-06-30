// NEARMISS func_00187CC0  (vram 0x00187CC0, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation + instruction-scheduling permutation (logic fully recovered, structure matches). Residuals: flag temp in a1 vs target v1; mwcc list-scheduler interleaves the 0x700038A8/AC zero-stores with the func_001026A0 arg-address setup differently; one idiom-13 redundant 'b' branch in th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Spawns/initializes an entity instance. Picks a 'mode' from func_0015D2F0
// status (2 or 0x82) and global flag D_00810525[0]==3, allocates via
// func_001F4F40(mode), copies two qword blocks from the entity at a0, seeds the
// 0x700038A0 scratch matrix block (col = 0.2,0.2,0,0) and runs the 0x10268x
// matrix helpers writing into the new instance.
extern int **D_00275B40;
extern unsigned char D_00810525[16];
extern char D_700038A0[];
extern char D_700038B0[];
extern int func_0015D2F0(void);
extern unsigned char *func_001F4F40(int);
extern int func_00102948(unsigned char *, unsigned char *);
extern int copy_qw4(unsigned char *, unsigned char *);
extern int func_001026A0(char *, unsigned char *, char *);
extern int func_001028B8(unsigned char *, unsigned char *, char *);

void func_00187CC0(unsigned char *a0) {
    int v0;
    int v1;
    int mode;
    unsigned char *p;

    v0 = func_0015D2F0();
    v1 = 1;
    if (v0 != 2) {
        v1 = 0;
        if (v0 == 0x82) {
            v1 = 1;
        }
    }
    if (D_00810525[0] == 3) {
        mode = 1;
        if (v1 != 0) {
            mode = 4;
        }
    } else {
        mode = v1 ? 3 : 0;
    }
    p = func_001F4F40(mode);
    if (p != 0) {
        func_00102948(p + 0xB0, a0 + 0xB0);
        copy_qw4(p + 0xD0, (unsigned char *)*D_00275B40 + 0x90);
        *(int *)0x700038A0 = 0x3E4CCCCD;
        *(int *)0x700038A4 = 0x3E4CCCCD;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0;
        func_001026A0(D_700038B0, (unsigned char *)*D_00275B40 + 0x90, D_700038A0);
        func_001028B8(p + 0x100, a0 + 0xB0, D_700038B0);
        *(int *)(p + 0x10C) = 0x3F800000;
    }
}
