// Tail-call thunk: forwards to func_00204B30 with the pointer advanced past
// the object's 0x48-byte header.
extern void func_00204B30(void *p);

void func_00206BE0(char *p) {
    func_00204B30(p + 0x48);
}
