// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Two-mode setup writing a transform into the D_700038A0/B0/C0 scratch blocks.
// flag==0 vs flag!=0 selects the first scratch vector (-/+18.0f vs +18.0f);
// func_001026A0 builds D_700038B0 from p+0xD0, then a second scratch vector
// (with z=14.5f) builds D_700038C0; func_001028B8 combines them; one scratch
// float (0x700038CC) is set to 1.0f. func_0019AFE0 then tests the result
// (mask 6). On a hit, if the global object at 0x700031D0 has byte[0x1A]==0x32
// and func_001782A0(p) succeeds, returns 1; otherwise 0.
//
// Matched with mwcc 2.3.3 (objdiff 100.0 vs expected). The pinned 991202 build
// only reaches 89.1% (clean-store delay-slot fill + tail branch lowering).
// Key idiom: the common 0x700038AC=1.0f store is duplicated into BOTH if/else
// arms so 2.3.3 schedules it inside the taken branch (matching the target's
// in-branch store + branch-delay-slot layout).
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001782A0(char *p);
extern int func_0019AFE0(char *p, void *b, void *c, int n);
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700031D0[];

int func_001787B0(char *p, int flag) {
    if (flag == 0) {
        *(int *)0x700038A0 = 0xC1900000;
        *(int *)0x700038A4 = 0x41900000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
    } else {
        *(int *)0x700038A0 = 0x41900000;
        *(int *)0x700038A4 = 0x41900000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
    }
    func_001026A0(D_700038B0, p + 0xD0, D_700038A0);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(int *)0x700038A8 = 0x41680000;
    *(int *)0x700038AC = 0;
    func_001026A0(D_700038C0, p + 0xD0, D_700038A0);
    func_001028B8(D_700038C0, D_700038C0, D_700038B0);
    *(int *)0x700038CC = 0x3F800000;
    if (func_0019AFE0(p, D_700038B0, D_700038C0, 7) & 6) {
        if (*(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x32 && func_001782A0(p) != 0) {
            return 1;
        }
    }
    return 0;
}
