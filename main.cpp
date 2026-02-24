#include <iostream>
#include <random>

int count_bigger(const std::vector<long> &vec, const int n) {
    int cnt = 0;
    for (const long i : vec) {
        if (i > n) cnt++;
    }
    return cnt;
}

long max(const std::vector<long> &vec) {
    long max = vec[0];
    for (const long i : vec) {
        if (i > max) max = i;
    }
    return max;
}

int main() {
    std::vector<long> vec(1000);
    for (int i = 0; i < 1000; i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(-1000, 1000);
        vec[i] = distrib(gen);

    }
    std::cout << count_bigger(vec, 500) << std::endl;
    std::cout << max(vec) << std::endl;
    return 0;
}