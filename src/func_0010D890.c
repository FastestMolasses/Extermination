// COMPILER: eegcc
// CFLAGS: -O2
// Varargs forwarder: spills the GPR arg-save area and passes a pointer to
// the start of the varargs (saved a1..t3) to func_0010D2C8. The (next_arg
// - 0x38) adjustment reproduces ee-gcc's EABI va_start, which biases the
// arg pointer back over the 7-dword (0x38) GPR save area.
typedef char *va_list;
extern void func_0010D2C8(int a0, va_list args);

void func_0010D890(int a0, ...) {
    va_list args = (va_list)__builtin_next_arg(a0) - 0x38;
    func_0010D2C8(a0, args);
}
