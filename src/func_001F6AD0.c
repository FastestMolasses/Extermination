// Builds an object via func_001F6760 and hands it to func_001F6640.
extern int func_001F6760(void);
extern void func_001F6640(int obj);

void func_001F6AD0(void) {
    func_001F6640(func_001F6760());
}
