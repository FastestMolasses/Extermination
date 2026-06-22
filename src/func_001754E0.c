// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Timer/angle gate on an actor (arg0), threshold arg1. If the actor's enabled
// flag (+0x23F) is set, compute angle = func_0011DF78(func_001B1470(yaw - pitch))
// where yaw=*(f32*)(arg0+0x26C), pitch=*(f32*)(arg0+0x24C); cache it at the HW
// address 0x70003A20. If that angle exceeds pi/2 (1.5707964f) bump the +0x28
// counter and re-arm the +0x2FE cooldown byte to 0x3C. If the flag is clear,
// instead gate on (D_00810E74 & *(u16*)0x70003B76) to do the same bump/re-arm.
// Return 1 once the +0x28 counter reaches arg1, else 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. Body is
// byte-identical under both; the sole 991202 residual is the final return-value
// lowering (CW emits the branchy two-return-block form; both mwcc builds emit
// the compact slt+movn). 2.3.3 reproduces the CW branch form, giving a
// byte-identical match. Verified objdiff 100.0% vs build/expected/func_001754E0.o.
extern float func_001B1470(float);
extern float func_0011DF78(float);
extern unsigned short D_00810E74;

int func_001754E0(char *arg0, int arg1) {
    float a;

    if (*(unsigned char *)(arg0 + 0x23F) != 0) {
        a = func_0011DF78(func_001B1470(*(float *)(arg0 + 0x26C) - *(float *)(arg0 + 0x24C)));
        *(float *)0x70003A20 = a;
        if (!(a <= 1.5707964f)) {
            *(short *)(arg0 + 0x28) = (short)(*(short *)(arg0 + 0x28) + 1);
            *(char *)(arg0 + 0x2FE) = 0x3C;
        }
    } else if (D_00810E74 & *(unsigned short *)0x70003B76) {
        *(short *)(arg0 + 0x28) = (short)(*(short *)(arg0 + 0x28) + 1);
        *(char *)(arg0 + 0x2FE) = 0x3C;
    }
    if (*(short *)(arg0 + 0x28) >= arg1) {
        return 1;
    }
    return 0;
}
