// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Spawns/attaches a particle or effect. Acquires a handle via
// func_001CA7B0(arg0+0x30, 10.0f); if it is negative, bails out. Then dispatches
// on the mode arg4 (a switch, NOT if/else - this drives the test order and block
// layout): mode 0 wires func_001C7900(arg0, arg1, 0x3F5, 0) then binds the handle
// to the resolved object via func_001CA940(handle, func_001C6120(D_0028A490[arg2],
// arg3)); mode 2 instead pushes a command through the gp-rel display object
// D_00275670: func_001CCF70(arg0+0x30) -> r, func_001CB760(&D_007635C0, r,
// *(D_00275670+0x1C)), func_001D8C20(1), func_001C7900(arg0, arg1, 0x3F5, 3),
// func_001D3990(func_001C6120(D_0028A490[arg2], arg3)), then writes a 0x10-byte
// command record at the buffer cursor (+3 byte = 0x60, +4 word = 0, +0 half = 0),
// advances the cursor by 0x10, and closes with func_001D8C20(0).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 78.41% (clean-store delay-slot fill family). Keys: a `switch (arg4)` (case 0
// then case 2) reproduces the target's `beq ==2` / `beqz ==0` test order with the
// default falling through to the epilogue; func_001CCF70 takes arg0+0x30 (its arg
// fills the case-2 dispatch branch delay slot); -sdatathreshold 8 keeps the
// command-object base D_00275670 gp-relative and re-reading `base` once lets mwcc
// reload only the +0x1C cursor per store. Verified TRUE objdiff 100.0 byte-identical
// vs expected.
extern int func_001CA7B0(char *p, float x);
extern void func_001C7900(char *a, char *b, int c, int d);
extern int func_001C6120(int a, int b);
extern void func_001CA940(int a, int b);
extern int func_001CCF70(char *p);
extern void func_001CB760(char *a, int b, int c);
extern void func_001D8C20(int a);
extern void func_001D3990(int a);
extern int D_0028A490[];
extern char *D_00275670;
extern char D_007635C0[];

void func_001F3E30(char *arg0, char *arg1, int arg2, int arg3, int arg4) {
    int handle;

    handle = func_001CA7B0(arg0 + 0x30, 10.0f);
    if (handle < 0) {
        return;
    }
    switch (arg4) {
    case 0:
        func_001C7900(arg0, arg1, 0x3F5, 0);
        func_001CA940(handle, func_001C6120(D_0028A490[arg2], arg3));
        break;
    case 2: {
        char *base;
        int r;

        r = func_001CCF70(arg0 + 0x30);
        func_001CB760(D_007635C0, r, *(int *)(D_00275670 + 0x1C));
        func_001D8C20(1);
        func_001C7900(arg0, arg1, 0x3F5, 3);
        func_001D3990(func_001C6120(D_0028A490[arg2], arg3));
        base = D_00275670;
        *(char *)(*(char **)(base + 0x1C) + 3) = 0x60;
        *(int *)(*(char **)(base + 0x1C) + 4) = 0;
        *(short *)(*(char **)(base + 0x1C) + 0) = 0;
        *(char **)(base + 0x1C) = *(char **)(base + 0x1C) + 0x10;
        func_001D8C20(0);
        break;
    }
    }
}
