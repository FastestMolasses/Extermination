// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Actor sub-state dispatcher. Reads the actor's state byte at +0xD and
// fans out to one of three handlers: 0 -> func_00188AC0, 0x10 ->
// func_00188B80, 0x15 -> func_00188C70. Any other value is a no-op.
// Returns void.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// lone residual under 991202 was wall #13 (it fills a clean-store delay
// slot with a safe li/lui where CW leaves a nop). The 2.3.3 build leaves
// that nop, so this readable C is byte-identical (objdiff 100%); 991202 is
// 96.15%.
extern void func_00188AC0(char *);
extern void func_00188B80(char *);
extern void func_00188C70(char *);

void func_00188A50(char *arg0) {
    switch (*(unsigned char *)(arg0 + 0xD)) {
    case 0:
        func_00188AC0(arg0);
        break;
    case 0x10:
        func_00188B80(arg0);
        break;
    case 0x15:
        func_00188C70(arg0);
        break;
    }
}
