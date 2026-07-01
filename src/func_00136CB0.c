// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_00136C40(int a0, void *a1, void *a2, float fa0);
extern char D_700038A0[];
extern char D_700038B0[];

void func_00136CB0(void *arg0, char *arg1) {
    if (!(*(char *)(arg1 + 0x5B) & 0x10)) {
        *(float *)0x700038A0 = 1.684f;
        *(float *)0x700038A4 = 3.623f;
        *(float *)0x700038A8 = 0.258f;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B0 = 0x80;
        *(int *)0x700038B4 = 0x50;
        *(int *)0x700038B8 = 0x30;
        *(int *)0x700038BC = 0x80;
        func_00136C40(0xF, D_700038A0, D_700038B0, 8.0f);
    }
    if (*(char *)(arg1 + 0x5B) & 1) {
        *(float *)0x700038A0 = 4.54f;
        *(float *)0x700038A4 = 0.033f;
        *(float *)0x700038A8 = -0.056f;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B0 = 0x30;
        *(int *)0x700038B4 = 0x80;
        *(int *)0x700038B8 = 0x30;
        *(int *)0x700038BC = 0x80;
        func_00136C40(0x19, D_700038A0, D_700038B0, 5.0f);
    }
    if (*(char *)(arg1 + 0x5B) & 2) {
        *(float *)0x700038A0 = 2.367f;
        *(float *)0x700038A4 = 5.537f;
        *(float *)0x700038A8 = 2.948f;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B0 = 0x30;
        *(int *)0x700038B4 = 0x80;
        *(int *)0x700038B8 = 0x30;
        *(int *)0x700038BC = 0x80;
        func_00136C40(3, D_700038A0, D_700038B0, 5.0f);
    }
    if (*(char *)(arg1 + 0x5B) & 4) {
        *(float *)0x700038A0 = 4.94f;
        *(float *)0x700038A4 = 0.052f;
        *(float *)0x700038A8 = -0.023f;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B0 = 0x30;
        *(int *)0x700038B4 = 0x80;
        *(int *)0x700038B8 = 0x30;
        *(int *)0x700038BC = 0x80;
        func_00136C40(0xD, D_700038A0, D_700038B0, 5.0f);
    }
}
