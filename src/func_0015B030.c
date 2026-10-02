// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State handler of an actor that follows a leader (self+0x20). State 0:
// when func_0015AC00(self) returns 0 and the leader's state is 2 or more, the
// state becomes 3. State 1: while the leader's state is below 2 it rebuilds its
// position from the leader (func_001028B8(self+0xB0, leader+0xB0, self+0xA0),
// w = 1.0), runs func_001C6380(self) and func_0015AE20(self, self+0x1F0);
// otherwise the state becomes 3. Other states: func_001B1190(self+0x9A) then
// func_001AFC10(self).
extern int func_0015AC00(void *actor);
extern void func_0015AE20(void *actor, void *sub);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001C6380(unsigned char *self);
extern void func_001B1190(int a0);
extern void func_001AFC10(void *self);

void func_0015B030(unsigned char *self) {
    unsigned char *sub = self + 0x1F0;
    unsigned char *leader = *(unsigned char **)(self + 0x20);
    switch (self[4]) {
    case 0:
        if (func_0015AC00(self) == 0 && !(leader[4] < 2)) {
            self[4] = 3;
        }
        break;
    case 1:
        if (!(leader[4] < 2)) {
            self[4] = 3;
        } else {
            func_001028B8(self + 0xB0, leader + 0xB0, self + 0xA0);
            *(float *)(self + 0xBC) = 1.0f;
            func_001C6380(self);
            func_0015AE20(self, sub);
        }
        break;
    case 2:
    case 3:
    default:
        func_001B1190(self[0x9A]);
        func_001AFC10(self);
        break;
    }
}
