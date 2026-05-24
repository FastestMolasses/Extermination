// CFLAGS: -O4,p -sdatathreshold 0
extern int D_0028A564;
extern void func_00200830(int);
extern void func_001CCB10(void);
extern void func_00200890(void);

void func_00200970(int a0) {
    if (a0 == 0) {
        func_00200830(D_0028A564);
        func_001CCB10();
        if (*(volatile unsigned char *)0x70003B90 == 2) {
            func_00200890();
            return;
        }
    } else {
        func_00200830(D_0028A564);
        func_00200890();
    }
}
