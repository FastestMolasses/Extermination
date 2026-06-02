// Runs func_001D4960 then func_001D4B10 on the same object.
extern void func_001D4960(int obj);
extern void func_001D4B10(int obj);

void func_001D4B20(int obj) {
    func_001D4960(obj);
    func_001D4B10(obj);
}
