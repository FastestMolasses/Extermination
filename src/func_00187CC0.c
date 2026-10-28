// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 88.95% NEARMISS).
// Fixes read from the original instructions:
// - the status flag is an if/else on (status == 2 || status == 0x82), which
//   puts it in the original's register;
// - the mode choice is a full if/else ladder (no preset-then-override), which
//   removes a duplicated conditional move at the join and fills the
//   null-check delay slot;
// - the scratch block is written through the named float extern D_700038A0
//   (0.2f, 0.2f, 0, 0) rather than literal addresses, so the scheduler may move
//   the D_00275B40 load above the zero stores as the original does;
// - callee prototypes follow their definitions in src/ (void returns).

// Spawns/initializes an entity instance. Picks a 'mode' from func_0015D2F0
// status (2 or 0x82) and global flag D_00810525[0]==3, allocates via
// func_001F4F40(mode), copies two qword blocks from the entity at a0, seeds the
// 0x700038A0 scratch matrix block (col = 0.2,0.2,0,0) and runs the 0x10268x
// matrix helpers writing into the new instance.
extern int **D_00275B40;
extern unsigned char D_00810525[16];
extern float D_700038A0[];
extern char D_700038B0[];
extern int func_0015D2F0(void);
extern unsigned char *func_001F4F40(int);
extern void func_00102948(void *, void *);
extern void copy_qw4(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern void func_001028B8(void *, void *, void *);

void func_00187CC0(unsigned char *a0) {
    int v0;
    int v1;
    int mode;
    unsigned char *p;

    v0 = func_0015D2F0();
    if (v0 == 2 || v0 == 0x82) {
        v1 = 1;
    } else {
        v1 = 0;
    }
    if (D_00810525[0] == 3) {
        if (v1 != 0) {
            mode = 4;
        } else {
            mode = 1;
        }
    } else if (v1 != 0) {
        mode = 3;
    } else {
        mode = 0;
    }
    p = func_001F4F40(mode);
    if (p != 0) {
        func_00102948(p + 0xB0, a0 + 0xB0);
        copy_qw4(p + 0xD0, (unsigned char *)*D_00275B40 + 0x90);
        D_700038A0[0] = 0.2f;
        D_700038A0[1] = 0.2f;
        D_700038A0[2] = 0.0f;
        D_700038A0[3] = 0.0f;
        func_001026A0(D_700038B0, (unsigned char *)*D_00275B40 + 0x90, D_700038A0);
        func_001028B8(p + 0x100, a0 + 0xB0, D_700038B0);
        *(float *)(p + 0x10C) = 1.0f;
    }
}
