// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Weapon charge-state tick (sibling of func_001513B0/func_001381D0): 12-bit charge
// field (x5 if 0x8000), updates counters/ammo on two state blocks; arg1[0x61] gate
// (s0) decides set-2 vs decrement path; calls range-check func_001C6160 < 65.0f.
extern int func_001B4CF0(char *a);
extern int func_001C6160(char *a);
extern int func_001EFE00(int a, char *b);
int func_00133A20(char *arg0, char *arg1) {
    short var_a0; short temp_v1; short temp_v1_2; int s0;
    if (*(short *)(arg0 + 0x36) == 0) return 0;
    if (*(unsigned char *)(arg1 + 0x6A) != 0) goto clear36;
    if (*(unsigned char *)(arg1 + 0x65) == 0) goto body;
clear36:
    *(short *)(arg0+0x36)=0; return 0;
body:
    *(char *)(arg0+0)=2;
    *(short *)(arg1+0x56)=0x78; *(short *)(arg1+0x5A)=0;
    if ((*(unsigned char *)(arg1+0x6C)==0)&&(*(short *)(arg0+0x36)&0x4000)) { *(char *)(arg1+0x6C)=0x3C; func_001EFE00(0x80000027,arg0); }
    s0 = *(unsigned char *)(arg1+0x61);
    if (s0==0) goto after_c6160;
    if (*(unsigned char *)(arg0+0x2C)!=4) goto after_c6160;
    if (!((float)func_001C6160(arg0) - *(float *)(arg0+0x3C) < 65.0f)) s0 = 0;
after_c6160:
    if (*(unsigned char *)(arg1+0x66)!=0) *(unsigned char *)(arg1+0x67)+=(unsigned char)(*(short *)(arg0+0x36)&0xFFF);
    else *(unsigned char *)(arg1+0x67)=(unsigned char)(*(short *)(arg0+0x36)&0xFFF);
    *(char *)(arg1+0x66)=0x14;
    temp_v1=*(short *)(arg0+0x36);
    var_a0 = temp_v1 & 0xFFF;
    if (temp_v1 & 0x8000) var_a0 = var_a0 * 5;
    temp_v1_2=*(short *)(arg0+0x34);
    if (temp_v1_2 <= var_a0) {
        *(char *)(arg0+0)=2; *(short *)(arg0+0x34)=0; *(char *)(arg0+4)=2; *(char *)(arg0+5)=0; *(char *)(arg0+6)=0;
        func_001B4CF0(arg0);
        if (s0!=0) return 1;
        if ((*(unsigned char *)(arg1+0x60)&2)==0) goto set5b;
        if ((*(short *)(arg0+0x36)&0x2000)==0) goto ret5;
    set5b:
        *(char *)(arg0+5)=1;
    ret5:
        return 1;
    }
    *(short *)(arg0+0x34)=(short)(temp_v1_2-var_a0);
    {
        short f = *(short *)(arg0+0x36);
        if (f & 0x2000) {
            *(char *)(arg0+4)=2; *(char *)(arg0+6)=0; *(char *)(arg1+0x6B)=0; *(char *)(arg1+0x67)=0;
            if (s0==0) goto s5one;
            *(char *)(arg0+5)=0;
            goto rdone;
        s5one:
            *(char *)(arg0+5)=1;
        rdone:
            return 1;
        }
        if ((*(unsigned char *)(arg1+0x67) >= 0x19) || (f & 0x8000)) {
            *(char *)(arg0+4)=2; *(char *)(arg0+6)=0; *(char *)(arg1+0x67)=0; *(char *)(arg1+0x6B)=0;
            if ((*(unsigned char *)(arg1+0x60)&2)==0) goto s5two;
            *(char *)(arg0+5)=0;
            goto rtwo;
        s5two:
            *(char *)(arg0+5)=1;
        rtwo:
            return 1;
        }
        if (f & 0x5000) *(char *)(arg1+0x6A)=0x1E;
        *(char *)(arg0+0)=1;
    }
    *(short *)(arg0+0x36)=0; return 0;
}
