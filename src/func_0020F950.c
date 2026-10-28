// NEARMISS func_0020F950  (vram 0x0020F950, 0x6D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0).
// 3-state menu/actor tick: `switch (m->mode)` (case 0 init, case 1 selection input,
// case 2 render-config), every case ends in `break` to one shared epilogue.
// Fixes this wave (2026-10-27, was 85.91%): switch instead of nested ifs; func_00210F30
// takes one argument (its definition); the decrement wrap is `if (sel == 0) sel = 10;
// else sel--;`; the increment wrap is a strict `sel > 10`; the selection loops are plain
// counted for-loops; D_002821B0/D_002821B4/D_00282240 are volatile (they are stored in
// source order); the scratchpad vector is the relocated extern D_70003600 with its
// differences computed into locals first; the result reads both components first.
// Remaining differences: the 11-entry availability sum swaps two registers (base
// pointer vs running total), and the two position minus row loads of the scratchpad
// vector are scheduled in the other order (z pair before x pair).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f);
extern char D_70003400[];
extern float D_70003600[];
extern char *func_001AFF10(void);
extern void func_001B0000(void);
extern void func_00207D00(int a, int b);
extern void func_00207D90(int slot, int b0, int b2, int b1, int b3);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CD80(void);
extern void func_0020CDA0(void);
extern void func_00210A00(int a);
extern void func_00210C00(int a);
extern void func_00210F30(char *a);
extern void func_00211400(char *a, int b);
extern void func_002101C0(void);
extern float D_002657C0[];
extern int D_00265890[];
extern volatile int D_002821B0;
extern volatile int D_002821B4;
extern int D_002821B8;
extern volatile int D_00282240;
extern float D_00810350;
extern float D_00810358;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_008106CD;
extern unsigned char D_00810700[];
extern unsigned char D_00810CB8[];
extern unsigned short D_00810E74;
extern unsigned short D_00810E78;

typedef struct {
    unsigned char pad0[3];
    unsigned char mode;      /* 0x03 */
    unsigned char pad4[0xC];
    char state;              /* 0x10 */
    unsigned char pad11;
    unsigned char sel;       /* 0x12 */
    unsigned char locked;    /* 0x13 */
    unsigned char sub;       /* 0x14 */
    unsigned char pad15[0xF];
    float scale;             /* 0x24 */
    float dz;                /* 0x28 */
    float dx;                /* 0x2C */
} Menu;

void func_0020F950(Menu *m) {
    switch (m->mode) {
    case 0: {
        int i;
        int total;

        for (i = 0; i < 0xB; i++) {
            char *p = func_001AFF10();
            *(void **)(p + 0x10) = &func_002101C0;
            p[3] = (char)i;
            p[0xD] = 0;
        }
        for (i = 0; i < 0xB; i++) {
            char *p = func_001AFF10();
            *(void **)(p + 0x10) = &func_002101C0;
            p[3] = (char)i;
            p[0xD] = 1;
        }
        m->mode++;
        D_002821B0 = 4;
        D_002821B4 = 0;
        D_00282240 = 6;
        total = 0;
        for (i = 0; i < 0xB; i++) {
            total += D_00810700[i + 0x5B8];
        }
        if (total != 0) {
            m->sel = D_008106CD & 0xF;
            if (D_00810CB8[m->sel] == 0) {
                int cnt;
                for (cnt = 0; cnt < 0xB; cnt++) {
                    if (m->sel == 0) m->sel = 0xA;
                    else m->sel--;
                    if (D_00810700[m->sel + 0x5B8] != 0) break;
                }
            }
        } else {
            m->sel = 0xFF;
        }
        m->sub = (D_008106CD & 0x30) >> 4;
        m->dx = 0.0f;
        m->dz = 0.0f;
        if (D_008106B0 != 0) {
            D_008106B0 = 0;
            m->sel = D_008106B1;
            m->locked = 1;
            m->scale = *(float *)((char *)D_002657C0 + m->sel * 0x14);
            m->mode = 2;
        } else {
            m->locked = 0;
            m->scale = 0.10666667f;
        }
        break;
    }
    case 1:
        func_00210A00(0);
        func_00210C00(0);
        func_00207D00(1, 3);
        func_001B0000();
        func_00211400((char *)m, 0);
        func_00207E40(1, 0x7080, 0x7900, 0x80, 0x40, 0x80808080, 0x200429859d322000ULL);
        if (D_00810E74 & 0x20) {
            func_0020CD60();
            m->state = 0x63;
            D_002821B4 = 0;
            break;
        }
        if (m->sel == 0xFF) break;
        if (m->locked == 0) {
            if (D_00810E78 & 0x2000) {
                int cnt;
                for (cnt = 0; cnt < 0xB; cnt++) {
                    m->sel++;
                    if (m->sel > 0xA) m->sel = 0;
                    if (D_00810700[m->sel + 0x5B8] != 0) {
                        func_0020CDA0();
                        break;
                    }
                }
            } else if (D_00810E78 & 0x8000) {
                int cnt;
                for (cnt = 0; cnt < 0xB; cnt++) {
                    if (m->sel == 0) m->sel = 0xA;
                    else m->sel--;
                    if (D_00810700[m->sel + 0x5B8] != 0) {
                        func_0020CDA0();
                        break;
                    }
                }
            }
        }
        if (D_00810E74 & 0x40) {
            if (D_00810CB8[m->sel] != 0) {
                func_0020CD40();
                m->locked = 1;
                m->scale = *(float *)((char *)D_002657C0 + m->sel * 0x14);
                if (D_00810700[0] == 0x12 || D_00810700[0] == 0x15) {
                    m->dz = 0.0f;
                    m->dx = 0.0f;
                } else if ((D_008106CD & 0xF) == m->sel) {
                    float *base = (float *)D_00265890[m->sel];
                    float *row = base + m->sub * 3;
                    float x, z;
                    z = D_00810358 - row[1];
                    x = D_00810350 - row[0];
                    *(float *)0x70003600 = x;
                    *(int *)0x70003604 = 0;
                    *(float *)0x70003608 = z;
                    D_70003600[3] = 1.0f;
                    func_001029C0(D_70003400);
                    func_00102BB0(D_70003400, D_70003400, -base[m->sub * 3 + 2]);
                    func_001026A0(D_70003600, D_70003400, D_70003600);
                    {
                        float z = D_70003600[2];
                        float x = D_70003600[0];
                        m->dx = 2.0f * (0.10666667f * x);
                        m->dz = 2.0f * (0.10666667f * -z);
                    }
                } else {
                    m->dz = 0.0f;
                    m->dx = 0.0f;
                }
                m->mode = 2;
            } else {
                func_0020CD80();
            }
        }
        D_002821B4 = 1;
        D_002821B8 = m->sel;
        break;
    case 2:
        func_00210A00(1);
        func_00207D00(1, 3);
        func_00207D90(1, 0x12, 9, 0x1EF, 0x96);
        func_001B0000();
        if ((D_008106CD & 0xF) == m->sel) {
            func_00210F30((char *)m);
        }
        func_00211400((char *)m, 1);
        func_00207D90(1, 0, 0, 0x200, 0xE0);
        func_00207E40(1, 0x7080, 0x7900, 0x80, 0x40, 0x80808080, 0x200429859d322000ULL);
        func_00210C00(1);
        if (D_00810E74 & 0x20) {
            func_0020CD60();
            m->locked = 0;
            m->dx = 0.0f;
            m->dz = 0.0f;
            m->mode = 1;
        }
        D_002821B4 = 1;
        D_002821B8 = m->sel;
        break;
    }
}
