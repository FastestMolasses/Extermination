// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Ring/stack pop: if count (D_00275BCC) > 0, decrement, pop *ptr (D_00275BD0) and advance; else return 0.
// blezl (branch-likely) + the 2.3.1-era dead `paddub v0,zero,zero` after the join are emitted
// by mwcc 2.3.3 from this if(>0){...return *p;}return 0; shape; 991202 lowers it differently.
extern short D_00275BCC;
extern int *D_00275BD0;

int func_001AF7C0(void) {
    int *p;
    if (D_00275BCC > 0) {
        D_00275BCC--;
        p = D_00275BD0;
        D_00275BD0 = p + 1;
        return *p;
    }
    return 0;
}
