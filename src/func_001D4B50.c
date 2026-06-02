// Runs func_001D49D0 then func_001D4B10 on the same object.
extern void func_001D49D0(int obj);
extern void func_001D4B10(int obj);

void func_001D4B50(int obj) {
    func_001D49D0(obj);
    func_001D4B10(obj);
}
