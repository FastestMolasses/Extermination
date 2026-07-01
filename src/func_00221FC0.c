// NEARMISS func_00221FC0  (vram 0x00221FC0, 0x5B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 89.97% — the state-machine dispatch/body logic is fully recovered (all 12 case bodies byte-verified against the disassembly, including the implicit-a0-forwarding calls func_0021D2E0/func_0021C120/func_0021C190/func_0021D490 that only take `e`). Two residual classes: (1) mwcc-vs-CW dispatch-chain ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern int func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern int func_00175900(unsigned char *e, int a1);
extern void func_00178B90(unsigned char *e, int a1);
extern void func_00179880(unsigned char *e, unsigned char *torso);
extern void func_0017FC80(unsigned char *e, float blend);
extern void func_0021C120(unsigned char *e);
extern int func_0021C190(unsigned char *e);
extern void func_0021C200(unsigned char *e);
extern void func_0021C270(unsigned char *e);
extern void func_0021C350(unsigned char *e);
extern void func_00182870(unsigned char *e, int f);
extern void func_001B61C0(int a0, int a1, int a2, int a3);
extern void func_001FBD50(unsigned char *e, int id, int a2, float radius);
extern void func_0021D250(unsigned char *e, int a1);
extern void func_0021D2E0(unsigned char *e, short a1, int a2);
extern void func_0021D490(unsigned char *e);
extern unsigned char **D_00275B40;
extern unsigned char D_008106F1[8];

void func_00221FC0(unsigned char *e) {
    unsigned char st;
    int s0;
    float zero;

    st = e[6];
    switch (st) {
    case 0:
        zero = 0.0f;
        func_001B61C0(0, 0xC0, 5, 1);
        e[6] = e[6] + 1;
        e[7] = 0;
        s0 = e[0x302];
        if (e[0xF] & 2) {
            s0 = 1;
            e[0xF] = 0;
        }
        if (*(float *)(e + 0x224) != zero) {
            func_001FBD50(e, 0x152, 0, 300.0f);
            func_0021C350(e);
        }
        if (*(float *)(e + 0x22C) != zero) {
            func_001FBD50(e, 0x153, 0, 300.0f);
            func_0021C270(e);
        }
        if (*(float *)(e + 0x220) <= 0.0f) {
            func_0021C200(e);
            e[6] = 0xA;
            return;
        }
        if (!(*(float *)(e + 0x228) < 100.0f) && D_008106F1[0] != 0) {
            e[6] = 0x14;
            s0 = 0;
        }
        if (e[0x2F1] == 0) {
            if (s0 == 0) {
                func_001749A0(e, 0x102, 0, 8.0f);
            } else {
                func_001749A0(e, 0x104, 0, 8.0f);
            }
        } else if (s0 == 0) {
            func_001749A0(e, 0x103, 0, 8.0f);
        } else {
            func_001749A0(e, 0x105, 0, 8.0f);
        }
        *(float *)(e + 0xB4) = *(float *)(e + 0x294);
        return;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[4] = 1;
            e[5] = 0xC;
            e[6] = 0;
            *(short *)(e + 0x20E) = 0x3C;
            func_0017FC80(e, 16.0f);
            return;
        }
        return;
    case 10:
        e[6] = st + 1;
        func_001749A0(e, 0x106, 0, 8.0f);
        *(int *)(e + 0x21C) = 0;
        *(int *)(e + 0x2E4) = 0;
        return;
    case 11:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            e[6] = st + 1;
            return;
        }
        break;
    case 12:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
            return;
        }
        *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
        *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
        func_00178B90(e, 1);
        *(float *)(e + 0x2EC) = *(float *)(*D_00275B40 + 4) - *(float *)(e + 0x2E4);
        *(float *)(e + 0x2E4) = *(float *)(*D_00275B40 + 4);
        *(float *)(e + 0xB4) = *(float *)(e + 0xB4) + *(float *)(e + 0x2EC);
        func_00175900(e, 1);
        return;
    case 13:
        func_00179880(e, e + 0x2EC);
        if (func_00175900(e, 1) != 0) {
            func_00182870(e, 1);
            func_001FBD50(e, 0x156, 0, 300.0f);
            if (e[0xF] != 0x63) {
                if (e[0x234] == 1) {
                    e[4] = 2;
                } else {
                    e[6] = e[6] + 1;
                    func_001749A0(e, 0x2A, 0, 1.0f);
                    e[0x1F0] = 0x40;
                    func_001FBD50(e, 0x156, 0, 300.0f);
                    return;
                }
            } else {
                e[4] = 2;
            }
            e[5] = 3;
            e[6] = 0;
            e[0x1F0] = 0x3F;
            return;
        }
        if (e[0x23A] == 0x5D) {
            func_0021D250(e, 0);
        }
        return;
    case 14:
        if (*(float *)(e + 0x3C) <= 20.0f) {
            e[6] = st + 1;
            func_0021D490(e);
        }
        break;
    case 15:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
            e[7] = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 16:
        func_0021D2E0(e, 0x78, 0);
        return;
    case 20:
        if (*(float *)(e + 0x3C) <= 21.0f) {
            e[6] = st + 1;
            func_0021C120(e);
        }
        break;
    case 21:
        if (func_0021C190(e) != 0) {
            e[6] = e[6] + 1;
            return;
        }
        *(int *)(e + 0x204) = 0x3DCCCCCD;
        break;
    case 22:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[4] = 1;
            e[5] = 0xC;
            e[6] = 0;
            *(short *)(e + 0x20E) = 0x3C;
            func_0017FC80(e, 16.0f);
            return;
        }
        *(int *)(e + 0x204) = 0x3E800000;
        break;
    }
}
