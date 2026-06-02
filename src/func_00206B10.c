// Tail-call thunk: forwards to func_00204490 with the pointer advanced past
// the object's 0x48-byte header.
extern void func_00204490(void *p);

void func_00206B10(char *p) {
    func_00204490(p + 0x48);
}
