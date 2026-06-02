// Tail-call thunk: re-dispatches to func_001CAE40, passing the object's
// 0x44 field as the second argument.
extern void func_001CAE40(int *obj, int arg);

void func_001CAF60(int *obj) {
    func_001CAE40(obj, obj[0x11]);
}
