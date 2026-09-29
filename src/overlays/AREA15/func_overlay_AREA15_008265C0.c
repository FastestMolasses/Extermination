// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00826600 (splat/link name 008265C0; overlay code is
// linked 0x40 below where it runs), 0x244 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: state 0: +0x1F0 = -1, func_001B0FD0; when func_001BA1C0(self, 0x22) is
//  set lowers the +0x120 object's +0x84 by 20; func_0019C6F0(0x22, 1),
//  func_001C6380. State 1: state 3 when func_001BA1C0(self, 0x23) is set; with
//  D_70003B92 == 0: when 0x22 is set turns the +0x114 object +0x74 by
//  0.6108653 and the +0x118 object +0x70 by 0.69813174 (func_001B1470 each)
//  and func_001FC3C0(self, +0x1F0, 0x44F, 600, 4096); places (-2.575, 45.472,
//  -102.748) through self + 0xD0 into 0x700038B0, func_001F5940(2, .., 0),
//  func_001C6380, func_001A2370, func_001B1B70, +0x4C; with D_70003B92 set
//  func_001FC520(+0x1F0). States 2/3 func_0019C6F0(0x22, 0), func_001FC520,
//  func_001AFC10.
extern unsigned char D_70003B92;
extern float D_700038A0[];
extern float D_700038B0[];
extern int func_001B0FD0(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_0019C6F0(int a0, int a1);
extern void func_001C6380(unsigned char *self);
extern float func_001B1470(float a);
extern void func_001FC3C0(unsigned char *self, void *blk, int id, float f12, float f13);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001F5940(int a0, void *v, int a2);
extern void func_001A2370(unsigned char *self, void *p);
extern void func_001B1B70(unsigned char *self);
extern void func_001FC520(void *blk);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA15_008265C0(unsigned char *self) {
    int *blk = (int *)(self + 0x1F0);
    switch (self[4]) {
    case 0:
        *blk = -1;
        func_001B0FD0(self);
        if (func_001BA1C0(self, 0x22) != 0) {
            *(float *)(*(unsigned char **)(self + 0x120) + 0x84) -= 20.0f;
        }
        func_0019C6F0(0x22, 1);
        func_001C6380(self);
        break;
    case 1:
        if (func_001BA1C0(self, 0x23) != 0) {
            self[4] = 3;
            break;
        }
        if (D_70003B92 == 0) {
        if (func_001BA1C0(self, 0x22) != 0) {
            *(float *)(*(unsigned char **)(self + 0x114) + 0x74) += 0.6108653f;
            *(float *)(*(unsigned char **)(self + 0x114) + 0x74) =
                func_001B1470(*(float *)(*(unsigned char **)(self + 0x114) + 0x74));
            *(float *)(*(unsigned char **)(self + 0x118) + 0x70) += 0.69813174f;
            *(float *)(*(unsigned char **)(self + 0x118) + 0x70) =
                func_001B1470(*(float *)(*(unsigned char **)(self + 0x118) + 0x70));
            /* matching device: 4096.0 staged through an int orders f13 before f12 */
            { int zi = 4096; float b = (float)zi; func_001FC3C0(self, blk, 0x44F, 600.0f, b); }
        }
        *(float *)0x700038AC = 1.0f;
        *(float *)0x700038A0 = -2.575f;
        *(float *)0x700038A4 = 45.472f;
        *(float *)0x700038A8 = -102.748f;
        func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
        func_001F5940(2, D_700038B0, 0);
        func_001C6380(self);
        func_001A2370(self, self + 0xD0);
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        } else {
            func_001FC520(blk);
        }
        break;
    case 2:
    case 3:
        func_0019C6F0(0x22, 0);
        func_001FC520(blk);
        func_001AFC10(self);
        break;
    }
}
