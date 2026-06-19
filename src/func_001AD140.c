// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001FABB0(void);
extern void func_001FBC50(void);
extern void func_001FC9B0(void);

#define PTR (*(volatile unsigned char **)0x70003B6C)

void func_001AD140(void) {
    PTR[8] = 3;
    PTR[9] = 2;
    PTR[0xA] = 0;
    PTR[0xB] = 0;
    func_001FC9B0();
    func_001FBC50();
    func_001FABB0();
}
