// Tears down / resets a subsystem: runs a fixed sequence of unregister/reset
// calls, then clears the object's live state fields.
extern void func_0011A918(int, int, int);
extern void func_0011A888(int);
extern void func_0011A7F0(int);
extern void func_0011A770(void);

void func_00206320(int *obj) {
    func_0011A918(1, 0, 0);
    func_0011A918(2, 0, 0);
    func_0011A888(3);
    func_0011A7F0(0);
    func_0011A7F0(1);
    func_0011A770();
    obj[0] = 0;
    *(int *)((char *)obj + 0x2C) = 0;
    *(int *)((char *)obj + 0x34) = 0;
    *(int *)((char *)obj + 0x38) = 0;
    *(int *)((char *)obj + 0x40) = 0;
    *(int *)((char *)obj + 0x54) = 0;
    *(int *)((char *)obj + 0x4C) = 0;
    *(int *)((char *)obj + 0x50) = 0;
}
