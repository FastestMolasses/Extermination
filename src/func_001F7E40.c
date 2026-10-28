// NEARMISS func_001F7E40  (vram 0x001F7E40, 0x508 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC is now
// faithful (matching wave 2026-10-27, was 55.17%): the old body had the random sequence wrong.
// Remaining differences: in the sign/fade block the original keeps the +/-192 store to 0x70003A20
// and reloads it, reading it before the fade and the X word; mwcc here reads the fade first and
// forwards the stored constant (volatile fixes the branch but loses the fused product), shifting
// register colouring through the rest of the loop body.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Fixes read from the original instructions (wave 2026-10-27):
//  - Seven random draws per particle, each read from the seed BEFORE it advances
//    (seed = seed * 37 + 11); the old C advanced too rarely and re-used values.
//    The parity test for the sign takes its own draw.
//  - The +0x700 / +0x790 screen offsets are applied before the third draw.
//  - The sign is one ternary store; the faded product is stored and reused.
//  - The work block's fade is read through the work pointer (self + 8).
//
// Effect: state in *(u8*)(p+4); case 0 seeds the generator from func_00122BB8,
// a 60-frame timer at (p+0x1F4) and a 0 fade at (p+0x1F8), then falls into
// case 1, which emits 32 quads per frame through func_001F7CF0: position
// 512x224 random plus (0x700, 0x790), size 320..448 x 160..224, a +/-192
// horizontal shift scaled by fade*(0.7..1.0), the position centred on the size,
// colour 0x80 grey and alpha (32..48)*(1 - fade). When the timer runs out the
// fade ramps by 1/180 per frame and past 1.0 the effect goes to state 3;
// states 2/3 hand off to func_001AFC10.
extern int float_to_int(float);
extern int func_00122BB8(void);
extern void func_001AFC10(char *);
extern void func_001F7CF0(int *, int *, int *);
extern float D_70003A20;
extern int D_700038A0[];

void func_001F7E40(char *arg0) {
    int i;
    int n;
    char *self;
    int hi;
    int a2;
    float r;
    float fs;
    int a1, b4;

    self = arg0 + 0x1F0;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        *(int *)(self + 0) = func_00122BB8();
        *(int *)(self + 4) = 0x3C;
        *(float *)(self + 8) = 0.0f;
        *(unsigned char *)(arg0 + 4) = 1;
    case 1:
        n = *(int *)(self + 0);
        for (i = 0; i < 0x20; i++) {
            r = (float)((n >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            n = n * 37 + 11;
            D_700038A0[0] = float_to_int(512.0f * r);
            r = (float)((n >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            n = n * 37 + 11;
            D_700038A0[1] = float_to_int(224.0f * r);
            D_700038A0[0] = D_700038A0[0] + 0x700;
            D_700038A0[1] = D_700038A0[1] + 0x790;
            r = (float)((n >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            n = n * 37 + 11;
            D_700038A0[4] = float_to_int(320.0f + 128.0f * r);
            r = (float)((n >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            n = n * 37 + 11;
            D_700038A0[5] = float_to_int(160.0f + 64.0f * r);
            hi = (n >> 16) & 0xFFFF;
            n = n * 37 + 11;
            D_70003A20 = (hi & 1) ? -192.0f : 192.0f;
            r = (float)((n >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            n = n * 37 + 11;
            fs = D_70003A20 * (*(float *)(self + 8) * (0.7f + 0.3f * r));
            D_70003A20 = fs;
            D_700038A0[0] = float_to_int((float)D_700038A0[0] + fs);
            a2 = D_700038A0[4];
            a1 = D_700038A0[0];
            b4 = D_700038A0[5];
            D_700038A0[8] = 0x80;
            D_700038A0[9] = 0x80;
            D_700038A0[10] = 0x80;
            D_700038A0[0] = a1 - (a2 >> 1);
            D_700038A0[1] = D_700038A0[1] - (b4 >> 1);
            D_700038A0[4] = a2 + D_700038A0[0];
            D_700038A0[5] = D_700038A0[5] + D_700038A0[1];
            r = (float)((n >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            n = n * 37 + 11;
            D_700038A0[11] = float_to_int(32.0f + 16.0f * r);
            D_700038A0[11] = float_to_int((float)D_700038A0[11] * (1.0f - *(float *)(self + 8)));
            func_001F7CF0(&D_700038A0[0], &D_700038A0[4], &D_700038A0[8]);
        }
        *(int *)(self + 4) = *(int *)(self + 4) - 1;
        if (*(int *)(self + 4) < 0) {
            *(float *)(self + 8) = *(float *)(self + 8) + 0.0055555557f;
            if (!(*(float *)(self + 8) <= 1.0f)) {
                *(unsigned char *)(arg0 + 4) = 3;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
