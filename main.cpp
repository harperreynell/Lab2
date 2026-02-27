#include <iostream>
#include <random>

std::pair<int, int> count_bigger(const std::vector<long> &vec, const int n) {
    int cnt = 0;
    long max = vec[0];
    for (const long i : vec) {
        if (i > max) {
            max = i;
        }
        if (i > n) cnt++;
    }
    return std::make_pair(cnt, max);
}

int main() {
    std::vector<long> vec(1000);
    for (int i = 0; i < 1000; i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(-1000, 1000);
        vec[i] = distrib(gen);
    }

    std::pair<int,int> result = count_bigger(vec, 500);
    std::cout << result.first << std::endl;
    std::cout << result.second << std::endl;
    return 0;
}