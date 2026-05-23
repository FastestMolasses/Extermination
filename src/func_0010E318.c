/* lw $v1,0x10($a0); lui $v0,0xFFFE; ori $v0,$v0,0xFFFE; sw $zero,0x18($a0);
   and $v1,$v1,$v0; jr $ra; sw $v1,0x10($a0) */
void func_0010E318(int *a0) {
    int v1 = a0[0x10 / 4];
    a0[0x18 / 4] = 0;
    a0[0x10 / 4] = v1 & (int)0xFFFFFFFE;
}
