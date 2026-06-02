// Tail-call thunk: re-dispatches to func_001CB200, passing the object's
// 0x44 field as the second argument.
extern void func_001CB200(int *obj, int arg);

void func_001CB2B0(int *obj) {
    func_001CB200(obj, obj[0x11]);
}
