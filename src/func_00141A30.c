// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Weapon charge/reload tick: reads the 12-bit charge field at +0x36 (x5 when the
// 0x8000 bit is set), distributes it to the live ammo/timer counters on the second
// state block (arg1), and advances the weapon state machine (arg0 fields 0/4/5/6).
extern void func_001B4CF0(char *a);
extern void func_001EFE00(int a, char *b);
int func_00141A30(char *arg0, char *arg1) {
    short temp_v1; short var_s0; short temp_v1_2; short temp_a0;
    if (*(short *)(arg0 + 0x36) == 0) return 0;
    if ((*(char *)(arg1 + 0x88) != 0) || (*(char *)(arg1 + 0x89) != 0)) { *(short *)(arg0+0x36)=0; return 0; }
    *(char *)(arg0+0)=2;
    *(short *)(arg1+0x70)=0xF0;
    *(char *)(arg1+0x88)=-1;
    temp_v1 = *(short *)(arg0+0x36);
    var_s0 = temp_v1 & 0xFFF;
    if (temp_v1 & 0x8000) var_s0 = var_s0 * 5;
    if (*(char *)(arg1+0x8A)!=0) *(short *)(arg1+0x74)=(short)(*(short *)(arg1+0x74)+var_s0);
    else *(short *)(arg1+0x74)=var_s0;
    *(char *)(arg1+0x8A)=0x19;
    if ((*(char *)(arg1+0x8B)==0)&&(*(short *)(arg0+0x36)&0x4000)) { *(char *)(arg1+0x8B)=0x3C; func_001EFE00(0x80000027,arg0); }
    temp_v1_2=*(short *)(arg0+0x34);
    if (temp_v1_2 <= var_s0) {
        *(short *)(arg0+0x34)=0;
        *(char *)(arg1+0x8C)=0;
        *(char *)(arg0+4)=2; *(char *)(arg0+5)=2; *(char *)(arg0+6)=0;
        func_001B4CF0(arg0);
        if (!(*(char *)(arg1+0x80)&2)) goto set5;
        if (*(short *)(arg0+0x36)&0xA000) { set5: *(char *)(arg0+5)=1; }
        return 1;
    }
    *(short *)(arg0+0x34)=(short)(temp_v1_2-var_s0);
    temp_a0=*(short *)(arg0+0x36);
    if (temp_a0 & 0x2000) {
        *(char *)(arg0+4)=2; *(char *)(arg0+5)=1; *(char *)(arg0+6)=0; return 1;
    }
    if (*(unsigned char *)(arg0+4)==1) {
        if (*(short *)(arg1+0x74) < 0x19) goto check8000;
        goto setstate;
    }
check8000:
    if (temp_a0 & 0x8000) {
    setstate:
        *(char *)(arg0+4)=2; *(char *)(arg0+6)=0; *(short *)(arg1+0x74)=0;
        if (*(char *)(arg1+0x80)&2) *(char *)(arg0+5)=0;
        else *(char *)(arg0+5)=1;
        return 1;
    }
    if (temp_a0 & 0x5000) *(char *)(arg1+0x88)=0x1E;
    else *(char *)(arg1+0x88)=0;
    *(char *)(arg0+0)=1; *(short *)(arg0+0x36)=0;
    if (*(unsigned char *)(arg0+4)==1) {
        if ((int)*(unsigned char *)(arg0+5) < 5) { *(char *)(arg0+5)=6; *(char *)(arg0+6)=0; }
    }
    return 0;
}
