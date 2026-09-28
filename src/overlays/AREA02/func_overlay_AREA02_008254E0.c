// NEARMISS: not a code difference. The compiled object is byte-identical
// (overlay_match.py 100%), but it is the overlay's last function and covers
// two splat pieces; fill_overlay.py only lets a compiled object absorb
// pieces when it ends within 16 bytes of their slots, and the last slot runs
// on through the 0x30-byte zero pad before the text end (0x825640 link).
// Until that rule allows the text-end pad, the link takes this function from
// its splat pieces (still byte-identical).
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00825520 (splat/link name 008254E0; overlay
// code is linked 0x40 below where it runs), 0x130 bytes. Object byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Covers the splat pieces 008254E0, 00825520. Neither piece is absorbed at
// link time (see the NEARMISS note): the link assembles both.
// Role: when D_008104C4 is set, D_008102BA is nonzero and that object's
//  +0xD is 9: 0x700031F0 = 1; (D_00810350, D_00810358) is rotated about the
//  object's (+0xB0, +0xB8) by 0.00374 rad with func_0011DE90 and
//  func_0011E2A8, and D_00810374 = func_001B1470(0.00374 + D_00810374).
// Matching: D_00810350/58/74 are one extern array (keeps the store/load
//  order) and 0x700031F0 is an extern.
extern unsigned char *D_008104C4;
extern unsigned char D_008102BA;
extern float D_00810350[];
extern int D_700031F0;
extern float func_0011DE90(float);
extern float func_0011E2A8(float a);
extern float func_001B1470(float a);

void func_overlay_AREA02_008254E0(void) {
    unsigned char *o = D_008104C4;
    float t;
    float dx;
    float dz;

    if (o != 0 && D_008102BA != 0 && o[0xD] == 9) {
        D_700031F0 = 1;
        dx = D_00810350[0] - *(float *)(o + 0xB0);
        dz = D_00810350[2] - *(float *)(o + 0xB8);
        t = dx * func_0011DE90(0.00373999146f);
        D_00810350[0] = *(float *)(o + 0xB0) + (t + dz * func_0011E2A8(0.00373999146f));
        t = dx * -func_0011E2A8(0.00373999146f);
        D_00810350[2] = *(float *)(o + 0xB8) + (t + dz * func_0011DE90(0.00373999146f));
        D_00810350[9] = func_001B1470(0.00373999146f + D_00810350[9]);
    }
}
