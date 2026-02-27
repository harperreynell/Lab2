#include <iostream>
#include <random>

std::pair<int, int> calculate(const std::vector<long> &vec, const int n) {
    int cnt = 0;
    long max = vec[0];
    for (const long i : vec) {
        if (i > max) max = i;
        if (i > n) cnt++;
    }
    return std::make_pair(cnt, max);
}

std::mutex mtx;
std::pair<int, int> calculate_blocking(const std::vector<long> &vec, const int n, int &cnt, int &max) {
    int local_count = 0;
    int local_max = std::numeric_limits<int>::min();

    for (const long i : vec) {
        if (i > max) local_max = i;
        if (i > n) local_count++;
    }

    std::lock_guard lock(mtx);
    cnt += local_count;
    if (local_max > max) max = local_max;

    return std::make_pair(cnt, max);
}

std::atomic<int> atomic_cnt{0};
std::atomic<int> atomic_max{std::numeric_limits<int>::min()};
void find_cas(const std::vector<long> &vec, const int n) {
    for (const long i : vec) {
        if (i > n) {
            int expected_cnt = atomic_cnt.load();
            while (!atomic_cnt.compare_exchange_weak(expected_cnt, expected_cnt + 1)) {}

            int current_val = i;
            int expected_max = atomic_max.load();
            while (current_val > expected_max &&
                   !atomic_max.compare_exchange_weak(expected_max, current_val)) {}
        }
    }
}

int main() {
    int n = 500;
    std::vector<long> vec(500);
    for (int i = 0; i < 1000; i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(-1000, 1000);
        vec[i] = distrib(gen);
    }

    std::cout << "Single thread no blocking:\n";
    auto start = std::chrono::high_resolution_clock::now();
    std::pair<int,int> result = calculate(vec, n);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "More than " << n << " elements: " << result.first << std::endl;
    std::cout << "Max element: " << result.second << std::endl;
    std::cout << "Time: " << elapsed.count() << "s" << std::endl;

    int cnt = 0;
    int max = std::numeric_limits<int>::min();
    start = std::chrono::high_resolution_clock::now();
    result = calculate_blocking(vec, n, cnt, max);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "\nWith blocking:\n";
    std::cout << "More than " << n << " elements: " << result.first << std::endl;
    std::cout << "Max element: " << result.second << std::endl;
    std::cout << "Time: " << elapsed.count() << "s" << std::endl;

    std::cout << "\nCAS realisation:\n";
    start = std::chrono::high_resolution_clock::now();
    find_cas(vec, n);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "More than " << n << " elements: " << atomic_cnt.load() << std::endl;
    std::cout << "Max element: " << atomic_max.load() << std::endl;
    std::cout << "Time: " << elapsed.count() << "s" << std::endl;
    return 0;
}