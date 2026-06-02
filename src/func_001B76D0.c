// Invokes func_001B1E20 with the object's 0x14/0x18 fields and reports success.
extern void func_001B1E20(int a0, int a1);

int func_001B76D0(int a0, int a1, int *obj) {
    func_001B1E20(obj[5], obj[6]);
    return 1;
}
