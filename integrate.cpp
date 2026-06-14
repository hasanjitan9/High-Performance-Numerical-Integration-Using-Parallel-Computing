#include <iostream>
#include <cstdlib>
#include <chrono>
float func(float x);


using namespace std;



int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <a> <b> <n>" << endl;
        return 1;
    }

    float a = atof(argv[1]);
    float b = atof(argv[2]);
    int n = atoi(argv[3]);
    if (n <= 0) {
        cerr << "Usage: " << argv[0] << " <a> <b> <n>" << endl;
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    float step = (b - a) / n;
    float area = 0.0f;

    for (int i = 0; i < n; i++) {
        float xLeft = a + i * step;
        float xRight = xLeft + step;
        area += (func(xLeft) + func(xRight)) * 0.5f * step;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "Result: " << area << endl;
    cout << "Time: " << duration.count() << " microseconds" << endl;
    return 0;
}
