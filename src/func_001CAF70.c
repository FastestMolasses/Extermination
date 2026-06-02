// Runs func_001CAA00 on the object, then dispatches func_001CABA0 with the
// object's 0x90 field.
extern void func_001CAA00(int *obj);
extern void func_001CABA0(int *obj, int arg);

void func_001CAF70(int *obj) {
    func_001CAA00(obj);
    func_001CABA0(obj, obj[0x24]);
}
