// NEARMISS func_001AF8E0  (vram 0x001AF8E0, 0x170 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.18% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling / register-coloring permutation of the trailing gp-rel pointer-table block: mwcc reorders the address-load/store pairs (D_0028xxxx -> D_00275Bxx) and the var->var copies (e.g. D_00275B9C=D_00275BA0) into its own schedule with different temp-reg coloring vs CW. The free-list...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS (89.2% mwcc 2.3.3 / 82.7% 991202): pool/free-list init. Builds a
// 0x100-entry singly-linked free list over the D_007A5640 arena (0x2F0-byte
// stride; entry[+0x1C] = next), terminates with a 0 at index 0x100, then
// publishes the head/count (D_00275BC4/BC8 = base/0x100) and wires up a table
// of handler pointers (D_0028xxxx) into the gp-rel state block at D_00275Bxx,
// zeroing the paired 16-bit counters. Loop + linked-list build + arena
// terminator match 100%; sole residual is mwcc's instruction scheduling /
// register coloring of the trailing gp-rel pointer-table block (it reorders
// the address-load/store pairs and the var->var copies vs CW). Pure
// scheduling/regalloc permutation -> permuter territory.
extern void func_00121A28(void *, int, int);
extern int D_00275BC0;
extern int D_00275BBC;
extern void *D_00275BC4;
extern short D_00275BC8;
extern void *D_00275BB0;
extern void *D_00275BA0;
extern void *D_00275B90;
extern void *D_00275B54;
extern void *D_00275B80;
extern void *D_00275B9C;
extern void *D_00275B70;
extern void *D_00275BAC;
extern void *D_00275B8C;
extern void *D_00275B60;
extern short D_00275BB4;
extern short D_00275BB8;
extern short D_00275BA4;
extern short D_00275BA8;
extern short D_00275B94;
extern short D_00275B98;
extern short D_00275B58;
extern short D_00275B84;
extern short D_00275B88;
extern short D_00275B74;
extern short D_00275B78;
extern short D_00275B64;
extern short D_00275B68;
extern void *D_00275B7C;
extern void *D_00275B6C;
extern void *D_00275B5C;
extern char D_007A5640[];
extern char D_007A536C[];
extern char D_0028B020[];
extern char D_0028AFF0[];
extern char D_0028AF30[];
extern char D_0028AE30[];
extern char D_0028AC30[];
extern char D_0028AB30[];
extern char D_0028AAB0[];

void func_001AF8E0(void) {
    int i;
    char *p;

    p = D_007A5640;
    D_00275BC0 = 0;
    D_00275BBC = 0;
    i = 0;
    do {
        func_00121A28(p, 0, 0x2F0);
        *(char **)(p + 0x1C) = D_007A5640 + (i + 1) * 0x2F0;
        i += 1;
        p += 0x2F0;
    } while (i < 0x100);
    *(int *)(D_007A536C + i * 0x2F0) = 0;
    D_00275BC4 = D_007A5640;
    D_00275BC8 = 0x100;
    D_00275BB0 = D_0028B020;
    D_00275BA0 = D_0028AFF0;
    D_00275B90 = D_0028AF30;
    D_00275B54 = D_0028AAB0;
    D_00275B80 = D_0028AE30;
    D_00275B9C = D_00275BA0;
    D_00275B70 = D_0028AC30;
    D_00275BAC = D_00275BB0;
    D_00275B8C = D_00275B90;
    D_00275B60 = D_0028AB30;
    D_00275BB4 = 0;
    D_00275BB8 = 0;
    D_00275BA4 = 0;
    D_00275BA8 = 0;
    D_00275B94 = 0;
    D_00275B98 = 0;
    D_00275B58 = 0;
    D_00275B84 = 0;
    D_00275B88 = 0;
    D_00275B74 = 0;
    D_00275B78 = 0;
    D_00275B64 = 0;
    D_00275B68 = 0;
    D_00275B7C = D_00275B80;
    D_00275B6C = D_00275B70;
    D_00275B5C = D_00275B60;
}
