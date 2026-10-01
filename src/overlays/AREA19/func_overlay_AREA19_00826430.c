// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA19 overlay, runtime 0x00826470 (splat/link name 00826430; overlay code
//  is linked 0x40 below where it runs), 0x8C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: script callback (script 0x82C6E0, record 0x82C9A0): when D_008102EC
//  <= 23: func_001FBD50(self, 0x134, 0, 300.0) once (D_00275B08 = 1) and
//  returns D_008102BA != 0; otherwise 0.
extern int D_00275B08;
extern unsigned char D_008102BA[];
extern void func_001FBD50(unsigned char *self, int id, int a2, float vol);
int func_overlay_AREA19_00826430(unsigned char *self) {
    if (*(float *)0x8102EC <= 23.0f) {
        if (D_00275B08 != 1) {
            func_001FBD50(self, 0x134, 0, 300.0f);
            D_00275B08 = 1;
        }
        if (D_008102BA[0] == 0) {
            return 0;
        }
    } else {
        return 0;
    }
    return 1;
}
