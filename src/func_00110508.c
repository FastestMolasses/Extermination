// NEARMISS func_00110508  (vram 0x00110508, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.88% via ee-gcc 2.9-991111-01 (-O2). Identified as the libkernel IOP reboot
// routine (sceSifRebootIop shape): if strlen(img) + 11 > 0x50, print the "too long"
// message (func_00122B58 = printf, D_0026B9C0 format) with img and return 0; otherwise
// func_0010E088(0) / func_0010E248() (RPC init/exit pair), build "<D_0026B9B0 prefix>" +
// img in a 0x50-byte stack buffer and RETURN func_00110368(buf, 0) (the IOP reset; its
// result is the return value, not 0).
// Remaining differences: the original walks img itself (callee-saved, also the printf
// argument) in the second copy loop with no copy; walking img directly makes ee-gcc's
// gcse hoist the first *img load above the first loop, and a separate walker costs one
// register copy and swaps s0/s1 (img/prefix) and the dst register. Declaration orders,
// inline helpers and block-local walkers tried.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern char D_0026B9B0[];
extern char D_0026B9C0[];
extern int func_00122B58(char *fmt, ...);
extern int func_0010E088(int);
extern void func_0010E248(void);
extern int func_00110368(char *buf, int x);

int func_00110508(char *img) {
    char param[0x50];
    char *src = D_0026B9B0;
    char *dst;
    char *p;

    for (p = img; *p; p++)
        ;
    if (p + 11 - img > 0x50U) {
        func_00122B58(D_0026B9C0, img);
        return 0;
    }
    func_0010E088(0);
    func_0010E248();
    dst = param;
    while (*src) *dst++ = *src++;
    p = img;
    while (*p) *dst++ = *p++;
    *dst = 0;
    return func_00110368(param, 0);
}
