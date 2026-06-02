// Tail-call thunk: raises the calling thread to priority 5.
extern void ChangeThreadPriority(int prio);

void func_00203980(void) {
    ChangeThreadPriority(5);
}
