// NEARMISS func_001AF5C0  (vram 0x001AF5C0, 0xCC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Store-scheduling artifact: source order already matches the CW target exactly (C4,B2, floats 310/314/318/31C/330/334/338/33C, int-zeros 320/324, floats 328/32C, shorts 344/346), but both mwcc builds hoist the two 'sw zero' stores (320/324) up adjacent to the earlier 'sb zero' (B2) instead of leav...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 81.5% (mwcc233). Body/logic fully recovered and in exact source order;
// sole residual is a store-scheduling artifact (see wall).
// Zero-inits the D_008102B0 struct (memset 0x320 via func_00121A28), stores its
// own address to D_008102C4, zeroes D_008102B2; sets eight 1.0f fields
// (310/314/318/31C/330/334/338/33C), two int-zero fields (320/324), two more 1.0f
// (328/32C); stores shorts -1 and 0x3D at 344/346; then func_001D8BF0(D_008102B0,1).
extern void func_00121A28(void *, int, int);
extern void func_001D8BF0(void *, int);

extern char D_008102B0[];
extern void *D_008102C4;
extern char D_008102B2;
extern float D_00810310, D_00810314, D_00810318, D_0081031C;
extern float D_00810330, D_00810334, D_00810338, D_0081033C;
extern int D_00810320, D_00810324;
extern float D_00810328, D_0081032C;
extern short D_00810344, D_00810346;

void func_001AF5C0(void) {
    func_00121A28(D_008102B0, 0, 0x320);
    D_008102C4 = D_008102B0;
    D_008102B2 = 0;
    D_00810310 = 1.0f;
    D_00810314 = 1.0f;
    D_00810318 = 1.0f;
    D_0081031C = 1.0f;
    D_00810330 = 1.0f;
    D_00810334 = 1.0f;
    D_00810338 = 1.0f;
    D_0081033C = 1.0f;
    D_00810320 = 0;
    D_00810324 = 0;
    D_00810328 = 1.0f;
    D_0081032C = 1.0f;
    D_00810344 = -1;
    D_00810346 = 0x3D;
    func_001D8BF0(D_008102B0, 1);
}
