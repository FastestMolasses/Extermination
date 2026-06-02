// Tail-call thunk: re-dispatches to func_001CB070, passing the object's
// 0x44 field as the second argument.
extern void func_001CB070(int *obj, int arg);

void func_001CB130(int *obj) {
    func_001CB070(obj, obj[0x11]);
}
