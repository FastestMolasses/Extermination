// NEARMISS func_00225AC0  (vram 0x00225AC0, 0x200 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.54% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Scheduling near-miss. Body byte-identical after goto-shared-return-0 reshape + large-array extern hack to keep D_00810040/D_00282157/D_0028A9A0 out of sdata while D_00275BD8 stays gp-rel (sdatathreshold 8). Two residual blocks differ only in instruction SCHEDULING: (1) arg-setup register ordering...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// State machine on global D_00810040 (byte[0]=main state, byte[1]=sub-state).
// NEARMISS 95.5% (mwcc 2.3.3): body byte-identical; sole residual is instruction
// scheduling in two blocks (arg-setup register order for func_001FF080, and a
// store scheduled into the func_001FBC50 jal delay slot). Logic fully recovered.
// The [64] array types on the externs are a matching device: they keep the large
// globals out of .sdata (forcing %hi/%lo) at sdatathreshold 8 while the 1-byte
// D_00275BD8 still resolves gp-relative, matching the original codegen.
extern int func_001FBC50(int, int);
extern int func_001FABB0(void);
extern int func_001FF080(int, int, int);
extern int func_001AEDE0(int, int);
extern int func_001AEE10(int, int, int);
extern int func_0020A7A0(long long);
extern int func_00225720(unsigned char *);
extern unsigned char func_00225D20(unsigned char *);
extern int func_00200970(int);
extern unsigned char D_00810040[64];
unsigned char D_00275BD8;
extern signed char D_00282157[64];
extern short D_0028A9A0[64];

unsigned char func_00225AC0(int arg0) {
    unsigned char *p = D_00810040;

    switch (p[0]) {
    case 0:
        switch (p[1]) {
        case 0:
            switch (arg0) {
            case 0:
                p[0x14] = 2;
                func_001FBC50(2, 3);
                func_001FABB0();
                p[1] = p[1] + 1;
                break;
            case 1:
                p[0x14] = 1;
                p[1] = 2;
                break;
            }
            break;
        case 1:
            if (D_00282157[0] != 0) {
                break;
            }
            p[1] = p[1] + 1;
        case 2:
            p[1] = p[1] + 1;
            D_00275BD8 = 1;
            func_001FF080(0, 0x2A, 3);
            break;
        case 3:
            if (D_00275BD8 == 0) {
                p[0] = p[0] + 1;
                p[1] = 0;
                func_001AEE10(8, 0, 3);
            }
            break;
        }
        goto d0;
    case 1:
        func_0020A7A0(((long long)0x20044C05 << 0x20) | ((long long)0x9D42 << 0x10) | 0x21E0);
        func_00225720(p);
        p[0x16] = func_00225D20(p);
        if (p[0x16] != 0) {
            func_001AEDE0(8, 0);
            p[0] = 2;
        }
        goto d0;
    case 2:
        func_0020A7A0(((long long)0x20044C05 << 0x20) | ((long long)0x9D42 << 0x10) | 0x21E0);
        func_00225720(p);
        if (D_0028A9A0[0] == 2) {
            if (*(unsigned char *)0x70003B90 == 2) {
                func_00200970(1);
            }
            p[0] = 3;
        }
        goto d0;
    case 3:
        return p[0x16];
    }
d0:
    return 0;
}
