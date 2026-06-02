// Tear down a pending sub-object (mark it state 3 and detach), then run the
// standard dispatch: notify subsystem of byte[0x9A] and process the object.
extern void func_001B1190(int);
extern void func_001AFC10(void *);

void func_00131E80(unsigned char *obj) {
    unsigned char *pending = *(unsigned char **)(obj + 0x20);
    if (pending) {
        pending[4] = 3;
        *(void **)(obj + 0x20) = 0;
    }
    func_001B1190(obj[0x9A]);
    func_001AFC10(obj);
}
