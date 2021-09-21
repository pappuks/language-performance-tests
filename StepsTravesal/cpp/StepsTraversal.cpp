#include <iostream>
#include <string.h>
#include <chrono>

using namespace std;

long jump(int remainingSteps)
{
    if (remainingSteps > 0)
    {
        return jump(remainingSteps - 1) + jump(remainingSteps - 2) + jump(remainingSteps - 3);
    }
    else
    {
        return 1;
    }
}

int main()
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    cout << jump(30) << endl;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}
