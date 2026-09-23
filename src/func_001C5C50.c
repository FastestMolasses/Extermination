// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane, 80.0% -> 100%): mwcc 2.3.3 places the +0xD
// store where the target does; 991202 scheduled it one slot later.
//
// Semantics (target 0x001C5C50): get a node pointer from func_001AFA90(8);
// if nonzero set its byte +3 = 3, byte +0xD = 0 and its word +0x10 to the
// handler func_001C5930. Same shape as func_0018A880.

extern void func_001C5930(void);
extern char *func_001AFA90(int);

void func_001C5C50(void) {
    char *p = func_001AFA90(8);
    if (p != 0) {
        p[3] = 3;
        p[0xD] = 0;
        *(void (**)(void))(p + 0x10) = func_001C5930;
    }
}
