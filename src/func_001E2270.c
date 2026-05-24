typedef unsigned __int128 uint128;
struct S_00275670 { char _pad[0x1C0]; uint128 field_1C0; };
extern struct S_00275670 *D_00275670;

void func_001E2270(uint128 *a0) {
    D_00275670->field_1C0 = *a0;
}
