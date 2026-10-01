// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00826540 (splat/link name 00826500; overlay code
//  is linked 0x40 below where it runs), 0x30 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: script callback (script 0x82C6E0, record 0x82C8A0):
//  func_0011A070(+0x2EC) unless it is -1; returns 1.
extern void func_0011A070(int h);

int func_overlay_AREA19_00826500(unsigned char *self) {
    int h = *(int *)(self + 0x2EC);
    if (h != -1) {
        func_0011A070(h);
    }
    return 1;
}
