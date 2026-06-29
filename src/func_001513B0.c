// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Weapon charge-state tick: reads the 12-bit charge field (x5 when high bit set),
// updates the live/target charge counters and ammo/timer fields on two state blocks.
extern int func_001B4CF0(char *a);
extern int func_001EFE00(int a, char *b);
int func_001513B0(char *arg0, char *arg1) {
    short temp_v1; short temp_v1_2; short temp_v1_3; short var_s0;
    if (*(short *)(arg0 + 0x36) == 0) return 0;
    if (*(char *)(arg1 + 0x60) != 0) { *(char *)(arg0+0)=1; *(short *)(arg0+0x36)=0; return 0; }
    *(char *)(arg0+0)=2; *(char *)(arg1+0x60)=-1;
    temp_v1 = *(short *)(arg0+0x36);
    var_s0 = temp_v1 & 0xFFF;
    if (temp_v1 & 0x8000) var_s0 = var_s0 * 5;
    if (*(char *)(arg1+0x61)!=0) *(short *)(arg1+0x50)=(short)(*(short *)(arg1+0x50)+var_s0);
    else *(short *)(arg1+0x50)=var_s0;
    *(char *)(arg1+0x61)=0x19;
    if ((*(char *)(arg1+0x62)==0)&&(*(short *)(arg0+0x36)&0x4000)) { *(char *)(arg1+0x62)=0x3C; func_001EFE00(0x80000027,arg0); }
    temp_v1_2=*(short *)(arg0+0x34);
    if (temp_v1_2 <= var_s0) { *(short *)(arg0+0x34)=0; func_001B4CF0(arg0); }
    else *(short *)(arg0+0x34)=(short)(temp_v1_2-var_s0);
    *(short *)(arg1+0x52)=(short)(*(short *)(arg1+0x52)+var_s0);
    if (*(unsigned char *)(arg0+4)==1) {
        if (*(short *)(arg0+0x34)!=0) {
            if ((*(short *)(arg1+0x50)>=0x19)||(*(short *)(arg0+0x36)&0xA000)) goto b22;
            goto b23;
        }
b22: *(unsigned char *)(arg0+4)=2; *(char *)(arg0+5)=0; *(char *)(arg0+6)=0; *(short *)(arg1+0x50)=0; *(char *)(arg1+0x60)=0x1E; return 1;
    }
b23: temp_v1_3=*(short *)(arg0+0x36);
    if (temp_v1_3&0x5000) *(char *)(arg1+0x60)=0xA;
    else if (temp_v1_3&0x2000) *(char *)(arg1+0x60)=0x1E;
    else *(char *)(arg1+0x60)=0;
    *(char *)(arg0+0)=1; *(short *)(arg0+0x36)=0; return 0;
}
