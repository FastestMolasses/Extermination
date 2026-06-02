// Tail-call thunk: re-dispatches to func_001CABA0, passing the object's
// 0x44 field as the second argument.
extern void func_001CABA0(int *obj, int arg);

void func_001CACB0(int *obj) {
    func_001CABA0(obj, obj[0x11]);
}
