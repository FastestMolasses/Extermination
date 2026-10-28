// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 74.38% NEARMISS).
// Fixes read from the original instructions:
// - The packed 5th argument is a plain unsigned int (not a 64-bit value); its low
//   24 bits are taken with a mask, which mwcc zero-extends from the raw register.
// - The string-table entry address is computed into a local before the two
//   context stores, so its loads are scheduled ahead of them.
// - D_00275828 / D_0027582C are one record (an int word and a byte after it);
//   writing both stores through the aggregate gives the original store schedule.
// - Callee prototypes take the string as a pointer (func_001CC170's definition
//   in src/readable takes a byte pointer).
//
// HUD text draw dispatcher (sibling of func_001FCBD0, same table walk).
// Looks up a string via the D_0028A498 table (base = tbl + *tbl, row = tbl+0x10,
// entry = base + row[idx*4]) resolved through func_001FE480(entry, arg3).
// 'flags' packs a context word: low 24 bits -> D_00275828.w, high byte ->
// D_00275828.b (+4, which splat names D_0027582C). arg2==8 selects centered HUD
// text: x = 0x800 - width/2 (width from func_001CC170) drawn with func_001CC1E0;
// otherwise the plain draw func_001FC770.

extern int func_001FE480(int *desc, int a1);
extern int func_001CC170(unsigned char *s);
extern int func_001CC1E0(int a0, int a1, int a2, int a3, int t0, char *t1, int *t2);
extern void func_001FC770(int a0, int a1, char *s, void *a3);
extern int *D_0028A498[8];
typedef struct Ctx { int w; char b; } Ctx;
extern Ctx D_00275828;
extern char D_00264CB0[];

void func_001FCE30(int arg0, int arg1, int arg2, int arg3, unsigned int flags) {
    int s;
    char *tbl;
    int *row;
    char *base;
    int *desc;

    tbl = (char *)D_0028A498[0];
    row = (int *)(tbl + 0x10);
    base = tbl + *(int *)tbl;
    desc = (int *)(base + row[arg2 * 4]);
    D_00275828.w = flags & 0xFFFFFF;
    D_00275828.b = (char)(flags >> 0x18);
    s = func_001FE480(desc, arg3);
    if (arg2 == 8) {
        func_001CC1E0(1, 0x800 - (func_001CC170((unsigned char *)s) >> 1), arg1 + 0x790,
                      0xA, 0x14, (char *)s, &D_00275828.w);
        return;
    }
    func_001FC770(arg0, arg1, (char *)s, D_00264CB0);
}
