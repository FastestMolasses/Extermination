// Computes a direction from the object's xy to the point, negates the x
// component, derives an angle via func_0011E620, and forwards it.
extern float func_0011E620(float dx, float dy);
extern void func_001B1470(float angle);

void func_001B1270(float *obj, float px, float py) {
    float dx = -1.0f * (px - obj[1]);
    float dy = py - obj[2];
    func_001B1470(func_0011E620(dx, dy));
}
