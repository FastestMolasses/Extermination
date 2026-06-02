// Tail-call thunk: re-dispatches to func_001CACC0, passing the object's
// 0x44 field as the second argument.
extern void func_001CACC0(int *obj, int arg);

void func_001CAE30(int *obj) {
    func_001CACC0(obj, obj[0x11]);
}
