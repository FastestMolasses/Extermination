// Tail-call thunk: appends a func_00205A50 record with word 0x3F and a zero
// 64-bit value at the packet cursor and returns the advanced cursor.
extern int *func_00205A50(int *a0, int a1, unsigned long long a2);
int *func_00205A80(int *a0) {
    return func_00205A50(a0, 0x3F, 0);
}
