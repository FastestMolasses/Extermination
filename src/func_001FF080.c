// CFLAGS: -O4,p -sdatathreshold 0
// Request a screen-module load through frame-task slot 2. Registering its
// callback clears the task state words first; then seed the requested state
// byte (+8) and DATA.DAT module/chunk id (+0xE) in that slot's record.
// The two globals lie outside the gp-relative window. The zero small-data
// threshold is required so their addresses use HI16/LO16 relocations.
typedef void (*FrameTask)(void);
extern void func_001FF0D0(void);
extern void func_001AB740(int slot_index, FrameTask callback);
extern unsigned char D_0028A798;
extern unsigned char D_0028A79E;

void func_001FF080(int initial_state, int module_id)
{
    func_001AB740(2, func_001FF0D0);
    D_0028A798 = initial_state;
    D_0028A79E = module_id;
}
