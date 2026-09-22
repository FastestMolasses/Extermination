// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Original interaction angle wrap. Its interval is(-pi,pi]: the
// negative-pi endpoint is deliberately converted to positive-pi.
// Ordinary C: byte-identical text and relocations, verified locally.
float func_001B1470(float angle)
{
    while (angle > 3.1415927f)
        angle -= 6.2831855f;
    while (angle <= -3.1415927f)
        angle += 6.2831855f;
    return angle;
}
