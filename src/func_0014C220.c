// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Per-frame tick for a sub-actor whose state record begins at arg0+0x1F0.
// Dispatches on the mode byte arg0[4] to one of four handlers, each called
// with (arg0, record): 0 func_0014C2E0 (spawn-init), 1 func_0014C3A0,
// 2 func_0014CCC0, 3 func_0014CD00 (mode 3 returns right after its handler).
// For modes 0-2 and any other value it then counts the signed byte timer at
// record+0x41 down by one when it is nonzero. Same shape as the byte-matched
// sibling func_0014D260.
extern void func_0014C2E0(char *, char *);
extern void func_0014C3A0(char *, char *);
extern void func_0014CCC0(char *, char *);
extern void func_0014CD00(char *, char *);

void func_0014C220(char *arg0) {
    char *p;
    int v;

    p = arg0 + 0x1F0;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        func_0014C2E0(arg0, p);
        break;
    case 1:
        func_0014C3A0(arg0, p);
        break;
    case 2:
        func_0014CCC0(arg0, p);
        break;
    case 3:
        func_0014CD00(arg0, p);
        return;
    }
    v = *(char *)(p + 0x41);
    if (v != 0) {
        *(char *)(p + 0x41) = v - 1;
    }
}
