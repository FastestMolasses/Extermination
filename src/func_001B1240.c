// Computes the relative vector (point - obj.xy) and feeds it through
// func_0011E620, passing the scalar result to func_001B1470.
extern float func_0011E620(float dx, float dy);
extern void func_001B1470(float v);

void func_001B1240(float *obj, float px, float py) {
    func_001B1470(func_0011E620(px - obj[0], py - obj[2]));
}
