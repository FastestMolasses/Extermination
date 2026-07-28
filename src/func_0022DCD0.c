// NEARMISS func_0022DCD0  (vram 0x0022DCD0, 0x95C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.66% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// beql+lui-likely-slot dispatch speculation (systematic, unreproducible): the target's 13-way sparse key dispatch emits beql with each single-pred case body's `lui vX,%hi(D_...)` in the LIKELY slot (plain beq only for shared 0/1 and 0x300/0x301 bodies) and speculates `lui at,%hi` from the taken pat...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001029C0(void *);
extern void func_00102B08(void *, void *, float);
extern void func_00102BB0(void *, void *, float);
extern int func_00122BB8(void);
extern void func_001AFC10(void *);
extern void func_001D0400(void *, void *, float);
extern void func_00102948(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern int func_001CCF70(void *);
extern int float_to_int(float);
extern void func_001CFB50(void *, int, void *, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);
extern void func_001FC3C0(void *, void *, int, float, float);

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_008101E4;
extern short D_0081024E;
extern float D_008105D0;

extern unsigned char D_00269400[];
extern unsigned char D_002695C0[];
extern unsigned char D_00269680[];
extern unsigned char D_00269840[];
extern unsigned char D_00269940[];
extern unsigned char D_00269D40[];
extern unsigned char D_00269FC0[];
extern unsigned char D_0026A240[];
extern unsigned char D_0026A340[];
extern unsigned char D_0026A3C0[];
extern unsigned char D_0026A540[];
extern unsigned char D_0026A5C0[];
extern unsigned char D_0026A640[];
extern unsigned char D_0026A900[];

extern unsigned char D_00822CF0[];
extern float D_008230F0[];
extern float D_008231F0[];
extern float D_00823230[];
extern float D_00823270[];
extern float D_008232B0[];
extern float D_008232F0[];
extern float D_00823330[];
extern float D_00823370[];
extern int D_008233B0[];
extern int D_008233F0[];
extern unsigned char D_00823430[];
extern float D_00823438;
extern float D_00823440;
extern unsigned char D_00823450[];
extern unsigned char D_00823460[];
extern int D_0082346C;
extern int D_008234B0;
extern float D_008234B4;
extern int D_700038A0;

void func_0022DCD0(unsigned char *p) {
    int st;
    int cnt;
    int seed;
    int *c;
    unsigned char flag;
    unsigned char *m;
    float *t;
    float *r;
    float *q;
    unsigned char *e;
    int i;
    int key;
    int n;
    int tmp;
    unsigned char *x;
    float fr;
    int h;
    float *colv;
    float *sizv;
    float *spdv;
    int *idv;
    float *rotv;
    float *phs2;
    int *texv;
    unsigned char buf[0x60];

    c = (int *)(p + 0x1F0);
    switch (*(unsigned char *)(p + 4)) {
    case 0:
        switch ((D_00810700 << 8) + D_00810701) {
        case 0x0:
        case 0x1:
            e = D_00269400;
            goto init;
        case 0x2:
            e = D_002695C0;
            goto init;
        case 0x200:
            e = D_00269680;
            goto init;
        case 0x202:
            e = D_00269840;
            goto init;
        case 0x300:
        case 0x301:
            e = D_00269940;
            goto init;
        case 0x600:
            e = D_00269D40;
            goto init;
        case 0x601:
            e = D_00269FC0;
            goto init;
        case 0x703:
            e = D_0026A240;
            goto init;
        case 0x800:
            e = D_0026A340;
            goto init;
        case 0x803:
            e = D_0026A3C0;
            goto init;
        case 0xD00:
            e = D_0026A540;
            goto init;
        case 0x1001:
            e = D_0026A5C0;
            goto init;
        case 0x1300:
            e = D_0026A640;
            goto init;
        }
        *(unsigned char *)(p + 4) = 3;
        break;
    init:
        *c = 0;
        while (*(int *)e != -1) {
            func_001029C0(D_00822CF0 + (*c << 6));
            x = D_00822CF0 + (*c << 6);
            func_00102B08(x, x, *(float *)(e + 0x2C));
            x = D_00822CF0 + (*c << 6);
            func_00102BB0(x, x, *(float *)(e + 0x30));
            *(float *)(D_00822CF0 + (*c << 6) + 0x30) = *(float *)(e + 0x20);
            *(float *)(D_00822CF0 + (*c << 6) + 0x34) = *(float *)(e + 0x24);
            *(float *)(D_00822CF0 + (*c << 6) + 0x38) = *(float *)(e + 0x28);
            D_008230F0[*c * 4] = (float)(int)*(unsigned char *)(e + 4);
            D_008230F0[*c * 4 + 1] = (float)((*(int *)(e + 4) >> 8) & 0xFF);
            D_008230F0[*c * 4 + 2] = (float)((*(int *)(e + 4) >> 16) & 0xFF);
            D_008230F0[*c * 4 + 3] = (float)((*(int *)(e + 4) >> 24) & 0xFF);
            D_008233B0[*c] = *(int *)e;
            D_00823330[*c] = *(float *)(e + 0x1C);
            D_008232B0[*c] = *(float *)(e + 0x10);
            D_008232F0[*c] = *(float *)(e + 0x14);
            D_00823370[*c] = *(float *)(e + 0x18);
            D_00823230[*c] = *(float *)(e + 8);
            D_00823270[*c] = *(float *)(e + 0xC);
            D_008231F0[*c] = 1.0f + (float)func_00122BB8() / 2147483648.0f;
            D_008233F0[*c] = -1;
            e += 0x40;
            (*c)++;
        }
        c[1] = func_00122BB8();
        *(unsigned char *)(p + 4) = 1;
        /* fallthrough */
    case 1:
        m = D_00822CF0;
        seed = c[1];
        q = D_00823230;
        colv = D_008230F0;
        sizv = D_008232B0;
        spdv = D_008232F0;
        idv = D_008233B0;
        r = D_00823270;
        rotv = D_00823370;
        t = D_008231F0;
        phs2 = D_00823330;
        i = 0;
        texv = D_008233F0;
        for (; i < *c; i++) {
            key = (D_00810700 << 8) + D_00810701;
            switch (key) {
            case 0xD00:
                if (D_00810702 != 4 && D_00810702 != 6) {
                    return;
                }
                break;
            case 0x1300:
                if (D_008101E4 == 3) {
                    if (D_0081024E == 8) {
                        return;
                    }
                } else if (D_008105D0 > 770.0f) {
                    if (*(float *)(m + 0x30) < 770.0f) {
                        goto next;
                    }
                } else {
                    if (*(float *)(m + 0x30) > 770.0f) {
                        goto next;
                    }
                }
                break;
            }
            func_001D0400(D_00823430, D_0026A900, *q);
            func_00102948(D_00823450, colv);
            func_00102948(D_00823460, colv);
            D_0082346C = 0;
            D_00823438 = *sizv;
            D_008234B4 = *spdv;
            D_008234B0 = *idv;
            n = float_to_int(*r);
            switch (n) {
            case 8:
                st = 1;
                cnt = 2;
                break;
            case 0xA:
                st = 1;
                cnt = 2;
                break;
            case 0xE:
                st = 3;
                cnt = 6;
                break;
            case 0x11:
                st = 3;
                cnt = 6;
                break;
            case 0x1C:
                st = 2;
                cnt = 5;
                break;
            case 0x23:
                st = 2;
                cnt = 8;
                break;
            default:
                st = 0;
                cnt = 1;
                break;
            }
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0;
            *(float *)0x700038A8 = 0.5f * *sizv;
            *(float *)0x700038AC = 1.0f;
            func_001026A0(&D_700038A0, m, &D_700038A0);
            h = func_001CCF70(&D_700038A0);
            while (st < cnt) {
                D_00823440 = *r * ((float)(st + 1) / (float)cnt);
                tmp = 1;
                if (!(*q < 4.0f)) {
                    tmp = 6;
                }
                flag = tmp;
                fr = (float)((seed >> 16) & 0xFFFF) / 65535.0f;
                seed = seed * 37 + 11;
                func_001CFB50(buf, 0, m, *t, fr + 0.0001f, 1.0f, 0.1f, *rotv);
                func_001CFBE0(h, flag, D_00823430, buf, 1);
                st++;
            }
            *t += *phs2;
            if (*t > 2.0f) {
                *t -= 1.0f;
            }
            *(float *)(p + 0xB0) = *(float *)(m + 0x30);
            *(float *)(p + 0xB4) = *(float *)(m + 0x34);
            *(float *)(p + 0xB8) = *(float *)(m + 0x38);
            *(float *)(p + 0xBC) = 1.0f;
            func_001FC3C0(p, texv, 0x420, 100.0f, 4096.0f);
        next:
            m += 0x40;
            q++;
            r++;
            colv += 4;
            t++;
            sizv++;
            spdv++;
            idv++;
            rotv++;
            phs2++;
            texv++;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(p);
        break;
    }
}
