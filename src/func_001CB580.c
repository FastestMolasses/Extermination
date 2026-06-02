// Tail-call thunk: re-dispatches to func_001CB4F0, passing the object's
// 0x44 field as the second argument.
extern void func_001CB4F0(int *obj, int arg);

void func_001CB580(int *obj) {
    func_001CB4F0(obj, obj[0x11]);
}
