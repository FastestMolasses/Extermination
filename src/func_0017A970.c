// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Input/state dispatcher keyed on two global mode bytes. arg1 is an
// edge/repeat flag. If D_00810CA4 == 2 (special mode) and arg1 == 0, forwards
// to func_0017A8B0(arg0, 5) and returns its result; otherwise returns 0.
// In the normal path it switches on D_00810CA6 (compared 4,1,3,2,0 in that
// order): cases 2/1 fire on arg1==0 calling func_0017A8B0(arg0, 1)/(arg0, 3),
// cases 3/4 fire on arg1==1 calling func_0017A8B0(arg0, 2)/(arg0, 4), and
// case 0 (arg1==0) toggles D_00810D3C: on the rising edge it kicks
// func_001FBD50(&D_008102B0, 0x179, 0, 300.0f) and sets D_008106C7=1; on the
// falling edge it clears D_00810D3C and D_008106C7. All non-firing paths
// return 0 through a shared epilogue.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build only
// reaches 72.6%. Two keys: (1) writing every non-call path as `break` to a
// single trailing `return 0;` reproduces the target's shared `paddub v0,zero`
// epilogue that all guards branch to; (2) folding the special-mode block as
// `if (D_00810CA4==2){...} else switch(...)` makes that block share the same
// epilogue instead of emitting a duplicate one. objdiff 100.0 vs expected.
extern int func_0017A8B0(char *arg0, int arg1);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char D_008102B0;
extern unsigned char D_008106C7;
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA6;
extern unsigned char D_00810D3C;

int func_0017A970(char *arg0, int arg1) {
    if (D_00810CA4 == 2) {
        if (arg1 == 0) {
            return func_0017A8B0(arg0, 5);
        }
    } else
    switch (D_00810CA6) {
    case 0:
        if (arg1 == 0) {
            if (D_00810D3C == 0) {
                D_00810D3C = 1;
                func_001FBD50(&D_008102B0, 0x179, 0, 300.0f);
                D_008106C7 = 1;
            } else {
                D_00810D3C = 0;
                if (D_008106C7 != 0) {
                    D_008106C7 = 0;
                }
            }
        }
        break;
    case 2:
        if (arg1 == 0) {
            return func_0017A8B0(arg0, 1);
        }
        break;
    case 3:
        if (arg1 == 1) {
            return func_0017A8B0(arg0, 2);
        }
        break;
    case 1:
        if (arg1 == 0) {
            return func_0017A8B0(arg0, 3);
        }
        break;
    case 4:
        if (arg1 == 1) {
            return func_0017A8B0(arg0, 4);
        }
        break;
    }
    return 0;
}
