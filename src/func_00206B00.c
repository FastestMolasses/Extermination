// Tail-call thunk: forwards to func_00204390 with the pointer advanced past
// the object's 0x48-byte header.
extern void func_00204390(void *p);

void func_00206B00(char *p) {
    func_00204390(p + 0x48);
}
