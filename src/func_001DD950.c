typedef unsigned __int128 uint128;
struct S_00275670 {
    char _pad2450[0x2450];
    uint128 field_2450;
    float field_2460;
    float field_2464;
};
extern struct S_00275670 *D_00275670;

void func_001DD950(uint128 *a0, float f12, float f13) {
    int *v1;
    float f0 = 16777215.0f / f12;
    D_00275670->field_2450 = *a0;
    v1 = (int *)D_00275670;
    *(float *)((char *)v1 + 0x2460) = f0;
    v1 = (int *)D_00275670;
    *(float *)((char *)v1 + 0x2464) = f13;
}
