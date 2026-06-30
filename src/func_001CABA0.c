// NEARMISS func_001CABA0  (vram 0x001CABA0, 0x110 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation/scheduling artifact. Body 100% logically complete & faithful. Keys that landed: (1) -sdatathreshold 4 makes D_00275B44 and D_00275670 gp-relative (matches target); (2) nested if (if(handle){ if(handle&1) D3D90 else D3990 } else D3990) reproduces the target beqz/andi dispatch o...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (93.3% on mwcc 2.3.3): body logically complete & faithful; residuals
// are register-allocation/scheduling artifacts only (handle/node s0<->s1 coloring
// swap; a gp-rel D_00275B44 load hoisted above the float-init branch). Permuter
// territory. -sdatathreshold 4 required so D_00275B44 / D_00275670 are gp-relative.
//
// Spawns/refreshes a UI or effect node. Computes a float param f = (arg1 ?
// *(float*)(arg1+0x20) : 20.0f) and calls handle = func_001CA7B0(D_00275B44+0xB0, f);
// bails if handle < 0. Caches node = *(D_00275670+0x1C), then func_001D8C20(1) and
// func_001C7420(arg0, 0x3F5, 3). Dispatches on handle: if handle!=0 pick
// func_001D3D90 (odd) or func_001D3990 (even); if handle==0 call func_001D3990 --
// all with arg1. Then writes a fixed DMA/packet header into *(D_00275670+0x1C):
// byte+3 = 0x60, word+4 = 0, half+0 = 0, advances the pointer by 0x10, calls
// func_001D8C20(0), and finishes with func_001CAAC0(D_00275B44+0xB0, node).

extern int func_001CA7B0(char *p, float x);
extern int func_001D8C20(int a0);
extern void func_001C7420(char *a0, int a1, int a2);
extern void func_001D3D90(char *a0);
extern void func_001D3990(char *a0);
extern int func_001CAAC0(char *a0, char *a1);

extern char *D_00275B44;
extern char *D_00275670;

void func_001CABA0(char *arg0, char *arg1)
{
    int handle;
    char *node;
    char *base;
    float f;
    int *p;

    if (arg1) {
        f = *(float *)(arg1 + 0x20);
    } else {
        f = 20.0f;
    }

    handle = func_001CA7B0(D_00275B44 + 0xB0, f);
    if (handle < 0) {
        return;
    }

    node = *(char **)(D_00275670 + 0x1C);
    func_001D8C20(1);
    func_001C7420(arg0, 0x3F5, 3);

    if (handle) {
        if (handle & 1) {
            func_001D3D90(arg1);
        } else {
            func_001D3990(arg1);
        }
    } else {
        func_001D3990(arg1);
    }

    base = D_00275670;
    p = *(int **)(base + 0x1C);
    *(char *)((char *)p + 3) = 0x60;
    p = *(int **)(base + 0x1C);
    *(int *)((char *)p + 4) = 0;
    p = *(int **)(base + 0x1C);
    *(short *)p = 0;
    p = *(int **)(base + 0x1C);
    *(int **)(base + 0x1C) = (int *)((char *)p + 0x10);
    func_001D8C20(0);

    func_001CAAC0(D_00275B44 + 0xB0, node);
}
