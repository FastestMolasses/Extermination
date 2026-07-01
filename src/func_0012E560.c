// NEARMISS func_0012E560  (vram 0x0012E560, 0x2DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.53% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/logic fully recovered (spawn-counter bump, owner-record init via two func_00102948 copies, D_00810808 flag OR, func_001B10B0 state gate chain, switch((flags)&0x83) timer table with D_0081070A-selected pairs, optional anim_clip_init kick). Modeled directly on the near-identical already-matche...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void func_00102948();
extern int func_001B10B0(char *a, int b, int c);
extern void bone_init_default_2(char *a, int b);
extern void anim_clip_init(char *self, int clip, float a, float b);
extern int D_00275388;
extern int D_0028A64C;
extern int D_0028A65C;
extern unsigned char D_0081070A;
extern unsigned char D_00810808;

void func_0012E560(char *arg0, char *arg1) {
    int a2;
    int v0;

    a2 = *(unsigned char *)(arg0 + 4) + 1;
    *(unsigned char *)(arg0 + 4) = a2;
    *(unsigned char *)(arg0 + 0) = 1;
    *(unsigned char *)(arg1 + 0x6D) = 1;
    *(unsigned char *)(arg1 + 0x6B) = 0;
    *(int *)(arg1 + 0x34) = 0x3F800000;
    a2 = *(unsigned char *)(arg0 + 0xD);
    *(unsigned char *)(arg1 + 0x6E) = a2;
    *(unsigned char *)(arg1 + 0x61) = 0;
    *(short *)(arg1 + 0x58) = 0;
    *(int *)(arg0 + 0x30) = (int)&D_00275388;
    *(int *)(arg0 + 0x20) = 0;
    func_00102948(arg1, arg0 + 0xB0, a2, 1);
    func_00102948(arg1 + 0x20, arg0 + 0xC0);
    if (D_00810808 == 0xFF) {
        *(unsigned char *)(arg0 + 0xD) = *(unsigned char *)(arg0 + 0xD) | 0x80;
    }
    if (*(unsigned char *)(arg0 + 0xD) & 1) {
        if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
            if (func_001B10B0(arg0, 0x70, 0x71) != 0) {
                return;
            }
            goto shared;
        }
        if (func_001B10B0(arg0, 0x6E, 0x71) != 0) {
            return;
        }
        v0 = D_0028A64C;
        goto set_and_bone;
    } else {
        if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
            if (func_001B10B0(arg0, 0x74, 0x71) != 0) {
                return;
            }
            goto shared;
        }
        if (func_001B10B0(arg0, 0x72, 0x71) != 0) {
            return;
        }
    }
shared:
    v0 = D_0028A65C;
set_and_bone:
    *(int *)(arg0 + 0x58) = v0;
    bone_init_default_2(arg0, 5);
    switch (*(unsigned char *)(arg0 + 0xD) & 0x83) {
    case 0:
        if (D_0081070A != 0) {
            *(short *)(arg0 + 0x34) = 0x78;
        } else {
            *(short *)(arg0 + 0x34) = 0x50;
        }
        break;
    case 0x80:
        if (D_0081070A != 0) {
            *(short *)(arg0 + 0x34) = 0xFA;
        } else {
            *(short *)(arg0 + 0x34) = 0x96;
        }
        *(int *)(arg0 + 0x60) = 0x3FA66666;
        *(int *)(arg0 + 0x64) = 0x3FA66666;
        *(int *)(arg0 + 0x68) = 0x3FA66666;
        *(int *)(arg0 + 0x6C) = 0x3F800000;
        break;
    case 1:
        if (D_0081070A != 0) {
            *(short *)(arg0 + 0x34) = 0x96;
        } else {
            *(short *)(arg0 + 0x34) = 0x64;
        }
        break;
    case 0x81:
        if (D_0081070A != 0) {
            *(short *)(arg0 + 0x34) = 0x12C;
        } else {
            *(short *)(arg0 + 0x34) = 0xB4;
        }
        *(int *)(arg0 + 0x60) = 0x3FA66666;
        *(int *)(arg0 + 0x64) = 0x3FA66666;
        *(int *)(arg0 + 0x68) = 0x3FA66666;
        *(int *)(arg0 + 0x6C) = 0x3F800000;
        break;
    case 3:
        if (D_0081070A != 0) {
            *(short *)(arg0 + 0x34) = 0x15E;
        } else {
            *(short *)(arg0 + 0x34) = 0xC8;
        }
        break;
    }
    if (*(short *)(arg0 + 0x56) != 0) {
        *(unsigned char *)(arg0 + 5) = 0;
        *(unsigned char *)(arg0 + 6) = 7;
        anim_clip_init(arg0, 5, 10.0f, 0.0f);
    }
}
