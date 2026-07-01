// NEARMISS func_001BA8E0  (vram 0x001BA8E0, 0x320 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 72.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NPC-spawner type dispatch (sparse compare-chain, no jump table). Body/mapping table fully recovered from the assembly; residual is register-allocation-order across the linear compare chain -- same class as the documented func_001BA080/func_001B99F0 'dense/sparse-switch lowering is compiler-intern...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern unsigned char D_00810788;
extern int D_0028A490[];
extern void func_001F0120(void *a0, int a1);
extern void func_001D8BF0(void *a0, int a1);
extern int func_001CA700(int p, int arg, int n);
extern void func_001D06D0(int p, int flag);

void func_001BA8E0(unsigned char *self, int type) {
    int idx;
    int s0;
    int v1;
    int s1;

    if (D_00810788 == 1 && type == 0x3B) {
        func_001F0120(self, 1);
    } else {
        func_001F0120(self, type);
    }
    func_001D8BF0(self, type);

    s0 = 1;
    v1 = 0;
    s1 = 1;

    switch (type) {
    case 0x6C:
        *(unsigned short *)(self + 0x96) = 0x34;
        *(unsigned short *)(self + 0x56) = 0;
        return;
    case 0x6A:
        *(unsigned short *)(self + 0x96) = 0x33;
        *(unsigned short *)(self + 0x56) = 0;
        return;
    case 0x68: s0 = 6; v1 = 0x31; s1 = 0; idx = -1;   break;
    case 0x66: s0 = 6; v1 = 0x30; idx = 0x8D;         break;
    case 0x64: s0 = 6; v1 = 0x2F; idx = 0x92;         break;
    case 0x61: s0 = 6; v1 = 0x2E; idx = 0x8C;         break;
    case 0x5E: s0 = 7; v1 = 0x2D; idx = 0x8B;         break;
    case 0x5D: s0 = 7; v1 = 0x2D; idx = 0x91;         break;
    case 0x5A: s0 = 7; v1 = 0x2C; s1 = 0; idx = 0x8A; break;
    case 0x59: s0 = 7; v1 = 0x2C; s1 = 0; idx = 0x94; break;
    case 0x55: s0 = 7; v1 = 0x2B; idx = 0x90;         break;
    case 0x54: s0 = 7; v1 = 0x2B; idx = 0x8F;         break;
    case 0x51: s0 = 7; v1 = 0x2A; idx = 0x89;         break;
    case 0x50: s0 = 7; v1 = 0x2A; idx = 0x95;         break;
    case 0x4F: s0 = 7; v1 = 0x2A; idx = 0x93;         break;
    case 0x49: s0 = 7; v1 = 0x29; idx = 0x8E;         break;
    case 0x48: s0 = 7; idx = 0x88;                    break;
    case 0x47: s0 = 7; idx = 0x88;                    break;
    case 0x40: idx = 0x1B;                            break;
    case 0x3F: idx = 0x1A;                            break;
    case 0x3E: idx = 0x19;                            break;
    case 0x3B: idx = 0x18;                            break;
    default:   idx = -1;                              break;
    }

    if (idx == -1) {
        *(unsigned short *)(self + 0x56) = 0;
        return;
    }

    *(unsigned short *)(self + 0x96) = v1;
    if (func_001CA700((int)self, D_0028A490[idx], s0) != 0) {
        func_001D06D0((int)self, s1);
        *(unsigned short *)(self + 0x56) = 1;
        return;
    }
    *(unsigned short *)(self + 0x56) = 0;
}
