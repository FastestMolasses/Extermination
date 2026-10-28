// NEARMISS func_001ECE70  (vram 0x001ECE70, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Corrected 2026-10-28 (was 80.58%): the fourth float argument is 0.1f (0x3DCCCCCD), not
// 1e-6; D_00275C34 is gp-relative at -sdatathreshold 4.
// Remaining differences: the original sets up 0.1f before 1.0f (and before the arg1 save);
// mwcc 2.3.3 sets up 1.0f first. Not fixed by a TU neighbour (func_001ECB00 after
// func_001ECA20), constant locals in any order, int-to-float conversion, or arg0 type.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Animation/effect setup wrapper: func_001CFB50(&D_0081F8F0, 0, arg0,
// D_00275C34->+0x54, D_00275C34->+0x5C, 1.0f, 0.1f, 9.0f), then
// func_001CFBE0(arg1, 1, D_00257000, &D_0081F8F0, 0).
extern int *D_00275C34;
extern char D_0081F8F0[0x100];
extern char D_00257000[0x100];
extern void func_001CFB50(void *, int, void *, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);

void func_001ECE70(void * arg0, int arg1) {
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  *(float *)((char *)D_00275C34 + 0x5C),
                  1.0f, 0.1f, 9.0f);
    func_001CFBE0(arg1, 1, D_00257000, D_0081F8F0, 0);
}
