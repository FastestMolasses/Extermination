// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA06 overlay, runtime 0x008242C0 (splat/link name 00824280; overlay code is
// linked 0x40 below where it runs), 0x80 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA06; lane A06C).
// Role: (self, arg) step on arg[4]: 0 sets D_008106C5 = 1, advances arg[4]
// and returns 0; 1 calls func_001FABB0 and func_001FB0B0(0) when bit 5 of
// D_00810845 is set and returns 1; other values return 0.
extern unsigned char D_008106C5;
extern unsigned char D_00810845;
extern void func_001FABB0(void);
extern void func_001FB0B0(int a);

int func_overlay_AREA06_00824280(unsigned char *self, unsigned char *a1) {
    switch (a1[4]) {
    case 0:
        D_008106C5 = 1;
        a1[4]++;
        break;
    case 1:
        if (D_00810845 & 0x20) {
            func_001FABB0();
            func_001FB0B0(0);
        }
        return 1;
    }
    return 0;
}
