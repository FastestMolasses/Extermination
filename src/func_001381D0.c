// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Weapon charge-state tick: reads 12-bit charge field (x5 if 0x8000 set, x4 if
// 0x1000), updates live/target charge counters and ammo/timer fields on two blocks.
extern int func_001B4CF0(char *a);
extern int func_001EFE00(int a, char *b);
int func_001381D0(char *arg0, char *arg1) {
    short var_s0; short temp_a0; short temp_v1; short temp_v1_2;
    if (*(short *)(arg0 + 0x36) == 0) return 0;
    if (*(char *)(arg1 + 0x40) != 0) { *(char *)(arg0+0)=1; *(short *)(arg0+0x36)=0; return 0; }
    *(char *)(arg0+0)=2; *(char *)(arg1+0x40)=-1;
    temp_a0 = *(short *)(arg0+0x36);
    var_s0 = temp_a0 & 0xFFF;
    if (*(char *)(arg1+0x44)!=0) *(short *)(arg0+0x36)=(short)(temp_a0 & 0x7FFF);
    temp_v1 = *(short *)(arg0+0x36);
    if (temp_v1 & 0x8000) var_s0 = var_s0 * 5;
    else if (temp_v1 & 0x1000) var_s0 = var_s0 * 4;
    if (*(char *)(arg1+0x42)!=0) *(short *)(arg1+0x30)=(short)(*(short *)(arg1+0x30)+var_s0);
    else { *(char *)(arg1+0x42)=0x2D; *(short *)(arg1+0x30)=var_s0; }
    if ((*(char *)(arg1+0x41)==0)&&(*(short *)(arg0+0x36)&0x4000)) { *(char *)(arg1+0x41)=0x3C; func_001EFE00(0x80000027,arg0); }
    temp_v1_2=*(short *)(arg0+0x34);
    if (temp_v1_2 <= var_s0) { *(short *)(arg0+0x34)=0; func_001B4CF0(arg0); }
    else *(short *)(arg0+0x34)=(short)(temp_v1_2-var_s0);
    {
        short f;
        if (*(short *)(arg0+0x34)==0) goto b4set2;
        if (*(short *)(arg1+0x30) >= 0xA0) goto b4set2;
        f = *(short *)(arg0+0x36);
        if (!(f & 0x8000)) goto chain;
    b4set2:
        *(char *)(arg0+4)=2; *(char *)(arg0+5)=0; *(char *)(arg0+6)=0; *(short *)(arg1+0x30)=0;
        return 1;
    chain:
        if (f & 0x5000) *(char *)(arg1+0x40)=0x1E;
        else if (f & 0x2000) *(char *)(arg1+0x40)=0x3C;
        else *(char *)(arg1+0x40)=0;
    }
done:
    *(char *)(arg0+0)=1; *(short *)(arg0+0x36)=0; return 0;
}
