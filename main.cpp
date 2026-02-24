#include <iostream>
#include <thread>
#include <atomic>

int main() {
    std::atomic<int> max;


    std::vector<long> vec(100);
    for (int i = 0; i < 100; i++) {
        vec[i] = random()%1000;
    }
    for (int i = 0; i < 100; i++) {
        std::cout << vec[i] << ' ';
    }
    std::cout << '\n';

    return 0;
}