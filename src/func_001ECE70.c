// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 80.61% NEARMISS).
// Fixes read from the original instructions:
//  - the 0.1 argument is set up before 1.0 (and before the arg1 save): in the
//    original source it is computed from an int local, tenth = 1.0 / (float)10
//    (idiom-24 / eps-lever shape); 1.0 and 9.0 stay plain constants.
//
// Animation/effect setup wrapper: func_001CFB50(&D_0081F8F0, 0, arg0,
// D_00275C34->+0x54, D_00275C34->+0x5C, 1.0f, 0.1f, 9.0f), then
// func_001CFBE0(arg1, 1, D_00257000, &D_0081F8F0, 0).
extern int *D_00275C34;
extern char D_0081F8F0[0x100];
extern char D_00257000[0x100];
extern void func_001CFB50(void *, int, void *, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);

void func_001ECE70(void * arg0, int arg1) {
    int tenth_i;
    float tenth;

    tenth_i = 10;
    tenth = 1.0f / (float)tenth_i;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  *(float *)((char *)D_00275C34 + 0x5C),
                  1.0f, tenth, 9.0f);
    func_001CFBE0(arg1, 1, D_00257000, D_0081F8F0, 0);
}
