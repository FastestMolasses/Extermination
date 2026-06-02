// Initialises the object's body (past its 0x48 header) and registers it,
// reporting success.
extern void func_00204AE0(char *body);
extern void func_001098D8(char *obj);

int func_00206B30(char *obj) {
    func_00204AE0(obj + 0x48);
    func_001098D8(obj);
    return 1;
}
