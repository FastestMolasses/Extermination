// CFLAGS: -O4,p -sdatathreshold 0
extern volatile char D_008102BB;
extern volatile char D_008104EA;
extern volatile char D_008105CE;

void func_00187EC0(char a0, char a1) {
    D_008102BB = 1;
    D_008104EA = a0;
    D_008105CE = a1;
}