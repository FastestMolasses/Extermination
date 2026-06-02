// Pick an animation clip id from one of two tables depending on a flag byte,
// then commit it via the clip setter (with the carried-through blend value).
extern short func_001885D0(unsigned char *e);
extern short func_001885F0(unsigned char *e);
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);

void func_0017FC80(unsigned char *e, float blend) {
    if (e[0x2F1] == 0) {
        func_001749A0(e, func_001885D0(e), 0, blend);
    } else {
        func_001749A0(e, func_001885F0(e), 0, blend);
    }
}
