// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Top-level per-frame entry for an actor whose control block lives at
// self+0x1F0. Dispatches the main state self+4: 0 -> func_0014ED10 (spawn
// init), 1 -> func_0014EDD0 (active update), 2 -> func_00150190, each with
// (self, self+0x1F0); state 3 -> func_00150820(self, self+0x1F0) and returns at
// once. For states other than 3 it then counts down the four timers ctx+0x61,
// ctx+0x54 (short), ctx+0x62, ctx+0x60 (each stops at 0) and calls
// func_001F91C0(self).
extern void func_0014ED10(char *arg0, char *arg1);
extern void func_0014EDD0(char *arg0, char *arg1);
extern void func_00150190(char *arg0, char *arg1);
extern void func_00150820(char *arg0, char *arg1);
extern void func_001F91C0(char *p);

void func_0014EC00(char *self) {
    char *ctx = self + 0x1F0;

    switch (*(unsigned char *)(self + 4)) {
    case 0:
        func_0014ED10(self, ctx);
        break;
    case 1:
        func_0014EDD0(self, ctx);
        break;
    case 2:
        func_00150190(self, ctx);
        break;
    case 3:
        func_00150820(self, ctx);
        return;
    }
    if (*(char *)(ctx + 0x61) != 0) {
        *(char *)(ctx + 0x61) = *(char *)(ctx + 0x61) - 1;
    }
    if (*(short *)(ctx + 0x54) != 0) {
        *(short *)(ctx + 0x54) = *(short *)(ctx + 0x54) - 1;
    }
    if (*(char *)(ctx + 0x62) != 0) {
        *(char *)(ctx + 0x62) = *(char *)(ctx + 0x62) - 1;
    }
    if (*(char *)(ctx + 0x60) != 0) {
        *(char *)(ctx + 0x60) = *(char *)(ctx + 0x60) - 1;
    }
    func_001F91C0(self);
}
