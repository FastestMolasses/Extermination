// Computes the relative vector (point - obj.xy) and feeds it through
// func_0011E620, passing the scalar result to func_001B1470 and returning
// that function's float result.
extern float func_0011E620(float dx, float dy);
extern float func_001B1470(float v);

float func_001B1240(float *obj, float px, float py) {
    return func_001B1470(func_0011E620(px - obj[0], py - obj[2]));
}
