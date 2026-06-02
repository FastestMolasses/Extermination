// Initialises the object's two sub-blocks (at +0xD0 and +0x110), tagging the
// second with the object's type byte.
extern void func_001029C0(char *block);
extern void func_001C9940(char *dst, int type, char *src);

void func_001C6960(char *obj) {
    func_001029C0(obj + 0xD0);
    func_001C9940(obj + 0x110, (unsigned char)obj[0xC], obj + 0xD0);
}
