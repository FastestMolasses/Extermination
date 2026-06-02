// Initialises subsystem 0 then queries func_00113680, returning 1 only when
// the query reports success (zero).
extern void func_00112D18(int id);
extern int func_00113680(void);

int func_00200700(void) {
    func_00112D18(0);
    return func_00113680() == 0 ? 1 : 0;
}
