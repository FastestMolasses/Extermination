extern int D_00275B48;
extern int D_00275B44;
extern void anim_bone_array_setup(int, int, int, int);

void func_001CB590(int a0, int a1, int a2, int a3) {
    D_00275B48 = a0;
    D_00275B44 = a0;
    anim_bone_array_setup(a2, a1, a2, a3);
}
