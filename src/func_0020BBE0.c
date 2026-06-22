// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Copies 5 bytes from a circular buffer at arg0+0x50 (length at arg0+0x18) into
// a linear window at arg0+0x90. The start index is the saved cursor at arg0+0x19;
// when arg1==1 it is decremented by one (wrapping to length-1 if it was 0). The
// read index wraps modulo the buffer length on each step.
void func_0020BBE0(char *arg0, int arg1) {
    int i;
    int idx;

    idx = *(unsigned char *)(arg0 + 0x19);
    if (arg1 == 1) {
        if (idx == 0) {
            idx = *(unsigned char *)(arg0 + 0x18) - 1;
        } else {
            idx -= 1;
        }
    }
    i = 0;
    do {
        *(unsigned char *)(arg0 + i + 0x90) = *(unsigned char *)(arg0 + idx + 0x50);
        idx += 1;
        if (idx >= *(unsigned char *)(arg0 + 0x18)) {
            idx = 0;
        }
        i += 1;
    } while (i < 5);
}
