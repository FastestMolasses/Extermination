// Dispatch helper: notify subsystem of this object's byte[0x9A], then process it.
// State-3 handler: the dispatcher passes (self, self+0x1F0) like every state
// handler; the sub-block pointer is unused.
extern void func_001B1190(int);
extern void func_001AFC10(void *);

void func_00150820(unsigned char *obj, char *st) {
    func_001B1190(obj[0x9A]);
    func_001AFC10(obj);
}
