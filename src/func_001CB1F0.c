// Tail-call thunk: re-dispatches to func_001CB140, passing the object's
// 0x44 field as the second argument.
extern void func_001CB140(int *obj, int arg);

void func_001CB1F0(int *obj) {
    func_001CB140(obj, obj[0x11]);
}
