// NEARMISS func_001FCBD0  (vram 0x001FCBD0, 0x260 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.81% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Packed-5th-arg extract wall — same proven wall as sibling func_001FCE30 (parked 74.4%), here reduced to 91.8: the target reads the packed context word bare out of $t0 (dsll32/dsrl32 ,8 low-24 + srl ,24 high byte, no operand conversions). Declaring the param unsigned long long makes both mwcc buil...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// HUD text-token draw dispatcher (sibling of func_001FCE30, same table walk).
// Looks up a string via the D_0028A498 table (base = tbl + *tbl, row = tbl+0x10,
// entry = base + row[idx*4]) resolved through func_001FE480(entry, arg3).
// The packed 5th arg (in $t0) is split into gp-rel scratch: low 24 bits ->
// D_00275828, high byte -> D_0027582C. arg2==8 selects the HUD-text path: a
// 128-byte template (D_00264D30) is block-copied to a stack buffer, then if the
// string contains a token character (func_00123280=strcspn vs func_001232E0=strlen)
// of the form 0x37..0x39 ('7'-'9') followed by a hex digit (classified via the
// ctype table D_0026C659: &4=digit, &1=upper hex, else lower hex), the two chars
// are packed into one byte ((first-'0')<<4 | nibble), spliced into the copy
// (func_00123418=strncpy prefix, byte, func_00122EF0=strcat of the tail skipping
// 4 chars) and drawn with func_001CC1E0(1, x+0x700, y+0x790, 10, 20, buf, ctx).
// Non-token / non-mode-8 strings go to the plain draw func_001FC770.

typedef unsigned __int128 uint128;

typedef union Buf128 {
    uint128 q[8];
    char b[0x80];
} Buf128;

extern int func_001FE480(int *desc, int a1);
extern int func_00123280(char *s, char *set);
extern int func_001232E0(char *s);
extern void func_00123418(char *dst, char *src, int n);
extern void func_00122EF0(char *dst, char *src);
extern int func_001CC1E0(int a0, int a1, int a2, int a3, int t0, char *t1, int *t2);
extern void func_001FC770(int a0, int a1, char *s, void *a3);
extern int *D_0028A498[8];
extern int D_00275828;
extern char D_0027582C;
extern char D_00264CB0[];
extern Buf128 D_00264D30;
extern char D_0026C659[];
extern char D_0026EC50[];

void func_001FCBD0(int arg0, int arg1, int arg2, int arg3, unsigned long long flags) {
    Buf128 buf;
    int s;
    int pos;
    char c;
    char *tbl;
    int *row;
    char *base;

    tbl = (char *)D_0028A498[0];
    row = (int *)(tbl + 0x10);
    base = tbl + *(int *)tbl;
    D_00275828 = flags << 0x28 >> 0x28;
    D_0027582C = (char)(flags >> 0x18);
    s = func_001FE480((int *)(base + row[arg2 * 4]), arg3);
    if (arg2 == 8) {
        buf = D_00264D30;
        pos = func_00123280((char *)s, D_0026EC50);
        if (pos != func_001232E0((char *)s)) {
            char *p = (char *)(pos + s) + 1;
            unsigned char c1 = *(unsigned char *)p;
            if ((c1 == 0x37) || ((unsigned int)(c1 - 0x38) < 2U)) {
                char hi = (char)(*p - 0x30);
                char ct;
                p = (char *)(s + pos) + 2;
                ct = D_0026C659[*(unsigned char *)p];
                if (ct & 0x44) {
                    if (ct & 4) {
                        c = (char)((hi << 4) | (*p - 0x30));
                    } else if (ct & 1) {
                        c = (char)((hi << 4) | (*p - 0x37));
                    } else {
                        c = (char)((hi << 4) | (*p - 0x57));
                    }
                    func_00123418(buf.b, (char *)s, pos);
                    buf.b[pos] = c;
                    func_00122EF0(buf.b, (char *)(s + (pos + 4)));
                    func_001CC1E0(1, arg0 + 0x700, arg1 + 0x790, 0xA, 0x14, buf.b, &D_00275828);
                    return;
                }
                func_001CC1E0(1, arg0 + 0x700, arg1 + 0x790, 0xA, 0x14, (char *)s, &D_00275828);
            }
        } else {
            func_001FC770(arg0, arg1, (char *)s, D_00264CB0);
        }
    } else {
        func_001FC770(arg0, arg1, (char *)s, D_00264CB0);
    }
}
