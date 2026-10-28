// NEARMISS func_001CABA0  (vram 0x001CABA0, 0x110 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.04% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Remaining differences: one instruction placement only. mwcc hoists the gp-relative
// D_00275B44 load above the arg1 test (into the entry block); the original loads it
// at the join just before the func_001CA7B0 call. Wave 2026-10-27 fixed the handle/node
// saved-register swap (twin func_001CACC0's `if (handle >= 0) { ... }` shape and its
// single-expression packet-header stores). A permuter run reached 100% only with a
// self-assignment of the float after the if/else (not honest C, rejected); copies into
// a second local, empty blocks, do/while(0), inline wrappers, volatile, a ternary,
// TU_STATIC func_001CA7B0 and other compilers did not stop the hoist.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

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
    float f;
    int handle;
    char *node;
    char *base;

    if (arg1 != 0) {
        f = *(float *)(arg1 + 0x20);
    } else {
        f = 20.0f;
    }
    handle = func_001CA7B0(D_00275B44 + 0xB0, f);
    if (handle >= 0) {
        node = *(char **)(D_00275670 + 0x1C);
        func_001D8C20(1);
        func_001C7420(arg0, 0x3F5, 3);
        if (handle != 0) {
            if (handle & 1) {
                func_001D3D90(arg1);
            } else {
                func_001D3990(arg1);
            }
        } else {
            func_001D3990(arg1);
        }
        base = D_00275670;
        *(char *)(*(char **)(base + 0x1C) + 3) = 0x60;
        *(int *)(*(char **)(base + 0x1C) + 4) = 0;
        *(short *)(*(char **)(base + 0x1C) + 0) = 0;
        *(char **)(base + 0x1C) = *(char **)(base + 0x1C) + 0x10;
        func_001D8C20(0);
        func_001CAAC0(D_00275B44 + 0xB0, node);
    }
}
