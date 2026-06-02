// Tail-call thunk: re-dispatches to func_001CAFA0, passing the object's
// 0x44 field as the second argument.
extern void func_001CAFA0(int *obj, int arg);

void func_001CB060(int *obj) {
    func_001CAFA0(obj, obj[0x11]);
}
