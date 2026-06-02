// Forwards into func_001C9610, deriving its arguments from sub-fields of the
// passed object before advancing the base pointer.
extern void func_001C9610(int *base, int flag, void *aux);

void func_001C63D0(char *obj) {
    int flag = (unsigned char)obj[0xC];
    void *aux = obj + 0xD0;
    func_001C9610((int *)(obj + 0x110), flag, aux);
}
