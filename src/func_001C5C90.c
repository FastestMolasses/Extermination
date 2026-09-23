// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003600 0x7000360C

//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x7000360C, 0x70003600
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// The `// SPAD:` directive above opts THIS file's expected object into the
// same %hi/%lo relocations (build.py _symbolize_scratchpad); the address stays
// out of the global _SPAD_SYMS because other matched files name the symbol
// yet store through literals. objdiff 100.0% (m2-matching lane).

//
// 4-state camera-track actor step, dispatched on the state byte at arg0+4,
// with the linked/parent record at *(arg0+0x18). State 0: if the parent's
// state byte (+4) >= 2, force own state 3; else func_001B1020(self,
// self[0xD], -1, -1) (start track by type id) and advance to state 1.
// State 1: same parent >= 2 guard (-> state 3); waits until the parent's
// ready byte (+9) is set; then a 10-value switch on the parent's type byte
// (+0xD) — types 0x47, 0x4E, 0x54/0x55, 0x58/0x59/0x5A, 0x5D/0x5E, 0x6A all
// copy the 4-quadword transform of the row *(parent+0x114) (offset +0x90)
// over the current row **D_00275B40 (offset +0x90); builds two probe points
// through the row transform via func_001026A0 and the spad vector at
// 0x70003600: {0,1,0,1} -> va and {5.4,1,0,1} -> vb; writes the direction
// arg0+0xC0 = vb - va (func_001028D0, a VU0 vsub of its 2nd minus 3rd
// argument) and normalizes it in place (func_00102760); stores va to arg0+0xA0..AC and vb to arg0+0xB0..BC with
// both w components forced to 1.0f; sets the active byte arg0+1 = 1; and if
// the parent's byte +1 is set, fires the virtual at arg0+0x4C. States 2/3:
// func_001AFC10(self) (release/teardown tick).
extern int D_7000360C[16];
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *src, void *mtx);
extern void func_00102760(void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_001AFC10(char *p);
extern void func_001B1020(char *p, unsigned char t, int a, int b);
extern char **D_00275B40;
extern int D_70003600[16];

void func_001C5C90(char *arg0) {
    unsigned char st;
    char *other;
    float va[4];
    float vb[4];

    st = *(unsigned char *)(arg0 + 4);
    other = *(char **)(arg0 + 0x18);
    switch (st) {
    case 0:
        if ((int)*(unsigned char *)(other + 4) >= 2) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        func_001B1020(arg0, *(unsigned char *)(arg0 + 0xD), -1, -1);
        *(unsigned char *)(arg0 + 4) = 1;
        break;
    case 1:
        if ((int)*(unsigned char *)(other + 4) >= 2) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        if (*(unsigned char *)(other + 9) == 0) {
            return;
        }
        switch (*(unsigned char *)(other + 0xD)) {
        case 0x47:
            copy_qw4(*D_00275B40 + 0x90, *(char **)(other + 0x114) + 0x90);
            break;
        case 0x4E:
            copy_qw4(*D_00275B40 + 0x90, *(char **)(other + 0x114) + 0x90);
            break;
        case 0x54:
        case 0x55:
            copy_qw4(*D_00275B40 + 0x90, *(char **)(other + 0x114) + 0x90);
            break;
        case 0x58:
        case 0x59:
        case 0x5A:
            copy_qw4(*D_00275B40 + 0x90, *(char **)(other + 0x114) + 0x90);
            break;
        case 0x5D:
        case 0x5E:
            copy_qw4(*D_00275B40 + 0x90, *(char **)(other + 0x114) + 0x90);
            break;
        case 0x6A:
            copy_qw4(*D_00275B40 + 0x90, *(char **)(other + 0x114) + 0x90);
            break;
        }
        D_70003600[0] = 0;
        *(int *)0x70003604 = 0x3F800000;
        *(int *)0x70003608 = 0;
        D_7000360C[0] = 0x3F800000;
        func_001026A0(va, *D_00275B40 + 0x90, D_70003600);
        D_70003600[0] = 0x40ACCCCD;
        *(int *)0x70003604 = 0x3F800000;
        *(int *)0x70003608 = 0;
        D_7000360C[0] = 0x3F800000;
        func_001026A0(vb, *D_00275B40 + 0x90, D_70003600);
        func_001028D0(arg0 + 0xC0, vb, va);
        func_00102760(arg0 + 0xC0, arg0 + 0xC0);
        *(float *)(arg0 + 0xA0) = va[0];
        *(float *)(arg0 + 0xA4) = va[1];
        *(float *)(arg0 + 0xA8) = va[2];
        *(float *)(arg0 + 0xAC) = va[3];
        *(int *)(arg0 + 0xAC) = 0x3F800000;
        *(float *)(arg0 + 0xB0) = vb[0];
        *(float *)(arg0 + 0xB4) = vb[1];
        *(float *)(arg0 + 0xB8) = vb[2];
        *(float *)(arg0 + 0xBC) = vb[3];
        *(int *)(arg0 + 0xBC) = 0x3F800000;
        *(char *)(arg0 + 1) = 1;
        if (*(unsigned char *)(other + 1) != 0) {
            (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
