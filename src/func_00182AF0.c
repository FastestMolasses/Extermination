// Play a sound (id = entity sound-base + 0x100) at full range (300.0).
extern int func_00179B90(void *);
extern void func_001FBD50(void *, int, int, float);

void func_00182AF0(void *obj) {
    int base = func_00179B90(obj);
    func_001FBD50(obj, base + 0x100, 0, 300.0f);
}
