// Stores the second argument into the object's 0x44 field, then forwards
// to func_001CA5F0 with the third argument promoted to the second slot.
extern void func_001CA5F0(int *obj, int arg);

void func_001CA5E0(int *obj, int field, int arg) {
    obj[0x11] = field;
    func_001CA5F0(obj, arg);
}
