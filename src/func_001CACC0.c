// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Spawns/updates a HUD or world FX node tied to actor arg0, scaled by arg1's
// scale field (+0x20, default 20.0, x1.2 if actor flag byte +0x2 bit 0x40 set).
// Allocates a node via func_001CA7B0(globalctx+0xB0, scale); on success grabs the
// owner's current node pointer (D_00275670->+0x1C), runs setup calls
// (func_001D8C20(1), func_001C7420(arg0,0x3F5,3)), dispatches an animation by
// handle parity (odd handle -> func_001D3DA0, else func_001D39A0, with arg1), then
// writes a terminator GIF/DMA tag into the owner ring (+0x1C: byte+3=0x60,
// word+4=0, half+0=0, advance +0x10), and finalizes with func_001D8C20(0). If the
// actor flag bit is set it routes through func_001CB760(&D_007635C0,0,node),
// otherwise func_001CAAC0(globalctx+0xB0,node).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): 233 hits 100.0 here, the pinned
// 991202 build only reaches 81.4 (wall #13 clean-store delay-slot nop). The gp-rel
// globals D_00275670/D_00275B44 require -sdatathreshold 8 (they sit in small data);
// D_007635C0 is a large object so it must be sized >8 to stay out of sdata
// (lui/%hi vs %gp_rel). The owner pointer is loaded ONCE after the dispatch
// if/else (single `owner = D_00275670`) so all +0x1C derefs share one base reg,
// matching the target. Verified objdiff 100.0 vs build/expected/func_001CACC0.o.
extern int func_001CA7B0(int, float);
extern void func_001CAAC0(int, char *);
extern void func_001CB760(char *, int, char *);
extern void func_001D39A0(int, char *);
extern void func_001D3DA0(int, char *);
extern void func_001D8C20();
extern void func_001C7420(char *, int, int);
extern char *D_00275670;
extern int D_00275B44;
extern char D_007635C0[256];

void func_001CACC0(char *arg0, char *arg1) {
    float scale;
    int handle;
    char *node;
    char *owner;

    if (arg1 != 0) {
        scale = *(float *)(arg1 + 0x20);
    } else {
        scale = 20.0f;
    }
    if (*(unsigned char *)(arg0 + 2) & 0x40) {
        scale *= 1.2f;
    }
    handle = func_001CA7B0(D_00275B44 + 0xB0, scale);
    if (handle >= 0) {
        node = *(char **)(D_00275670 + 0x1C);
        func_001D8C20(1);
        func_001C7420(arg0, 0x3F5, 3);
        if (handle != 0) {
            if (handle & 1) {
                func_001D3DA0(3, arg1);
            } else {
                func_001D39A0(3, arg1);
            }
        } else {
            func_001D39A0(3, arg1);
        }
        owner = D_00275670;
        *(char *)(*(char **)(owner + 0x1C) + 3) = 0x60;
        *(int *)(*(char **)(owner + 0x1C) + 4) = 0;
        *(short *)(*(char **)(owner + 0x1C) + 0) = 0;
        *(char **)(owner + 0x1C) = *(char **)(owner + 0x1C) + 0x10;
        func_001D8C20(0);
        if (*(unsigned char *)(arg0 + 2) & 0x40) {
            func_001CB760(&D_007635C0[0], 0, node);
            return;
        }
        func_001CAAC0(D_00275B44 + 0xB0, node);
    }
}
