// Dispatch helper: notify subsystem of this object's byte[0x9A], then process it.
extern void func_001B1190(int);
extern void func_001AFC10(void *);

void func_00136600(unsigned char *obj) {
    func_001B1190(obj[0x9A]);
    func_001AFC10(obj);
}
