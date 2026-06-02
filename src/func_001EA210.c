// Converts a float argument to int and stores it in table slot 2.
extern int float_to_int(float value);
extern void func_001D2DE0(int slot, int value);

void func_001EA210(float value) {
    func_001D2DE0(2, float_to_int(value));
}
