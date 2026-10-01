// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Status-indicator child node (port CENSUS_UNVERIFIED.md: em_indicator_child).
// State +4: 0 binds the child (func_001C22A0(node) == 0 means bound: place it with
// func_001C6380 and go to state 1); 1 copies the colour +0xA0..+0xAC to
// +0x80..+0x8C and draws it with func_001F54E0(node, node + 0x80) (unless
// bit 0 of the scratchpad flags 0x70003B68 is set, a copy of the colour with
// alpha 1.0 is first made on the stack and not used); any other state
// releases the node (func_001AFC10).
extern int D_70003B68;
extern int func_001C22A0(char *node);
extern void func_001C6380(char *node);
extern void func_00102948(void *dst, void *src);
extern void func_001F54E0(char *node, void *col);
extern void func_001AFC10(char *node);

void func_001C5760(char *node) {
    struct {
        float r, g, b;
        int a;
    } col;

    switch ((unsigned char)node[4]) {
    case 0:
        if (func_001C22A0(node) == 0) {
            func_001C6380(node);
            node[4] = 1;
        }
        break;
    case 1:
        *(float *)(node + 0x80) = *(float *)(node + 0xA0);
        *(float *)(node + 0x84) = *(float *)(node + 0xA4);
        *(float *)(node + 0x88) = *(float *)(node + 0xA8);
        *(float *)(node + 0x8C) = *(float *)(node + 0xAC);
        if (!(D_70003B68 & 1)) {
            func_00102948(&col, node + 0x80);
            col.a = 0x3F800000;
        }
        if (((unsigned char *)node)[0xA] != 0) {
            func_001C6380(node);
        }
        func_001F54E0(node, node + 0x80);
        break;
    case 2:
    case 3:
    default:
        func_001AFC10(node);
        break;
    }
}
