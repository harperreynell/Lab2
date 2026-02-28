#include <iostream>
#include <random>
#include <thread>

#define THRESHOLD 500
#define THREAD_COUNT 20

std::mutex mtx;
std::atomic atomic_cnt{0};
std::atomic atomic_max{std::numeric_limits<long>::min()};

std::pair<int, int> calculate(const std::vector<long> &vec, const int n) {
    int cnt = 0;
    long max = vec[0];
    for (const long i : vec) {
        if (i > max) max = i;
        if (i > n) cnt++;
    }
    return std::make_pair(cnt, max);
}

std::pair<int, int> calculate_blocking(const std::vector<long> &vec, const int n, int &cnt, long &max, int start, int end) {
    int local_count = 0;
    long local_max = std::numeric_limits<int>::min();

    for (int i = start; i < end; i++) {
        if (vec[i] > max) local_max = vec[i];
        if (vec[i] > n) local_count++;
    }

    std::lock_guard lock(mtx);
    cnt += local_count;
    if (local_max > max) max = local_max;

    return std::make_pair(cnt, max);
}

void calculate_atom(const std::vector<long> &vec, const int n, const int start, const int end) {
    for (int i = start; i < end; i++) {
        if (vec[i] > n) {
            int expected_cnt = atomic_cnt.load();
            while (!atomic_cnt.compare_exchange_weak(expected_cnt, expected_cnt + 1)) {}

            long current_val = vec[i];
            long expected_max = atomic_max.load();
            while (current_val > expected_max &&
                   !atomic_max.compare_exchange_weak(expected_max, current_val)) {}
        }
    }
}

double measure_time(const std::function<void()>& func) {
    const auto start = std::chrono::high_resolution_clock::now();
    func();
    const auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

int main() {
    for (std::vector<long> sizes = {10000, 100000, 500000, 1000000, 5000000, 10000000}; const auto size : sizes) {
        std::vector<long> vec(size);
        for (int i = 0; i < size; i++) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> distrib(-1000, 1000);
            vec[i] = distrib(gen);
        }
        double time_seq = measure_time([&]() { calculate(vec, THRESHOLD); });

        int mutex_cnt = 0;
        long mutex_max = std::numeric_limits<long>::min();
        double time_mutex = measure_time([&]() {
            std::vector<std::thread> threads;
            int chunk = size / THREAD_COUNT;
            for (int i = 0; i < THREAD_COUNT; ++i) {
                int start = i * chunk;
                int end = (i == THREAD_COUNT - 1) ? size : (i + 1) * chunk;
                threads.emplace_back(calculate_blocking, std::ref(vec), THRESHOLD, std::ref(mutex_cnt), std::ref(mutex_max), start, end);
            }
            for (auto& t : threads) t.join();
        });

        atomic_cnt = 0;
        atomic_max = std::numeric_limits<long>::min();
        double time_atom = measure_time([&]() {
            std::vector<std::thread> threads;
            int chunk = size / THREAD_COUNT;
            for (int i = 0; i < THREAD_COUNT; ++i) {
                int start = i * chunk;
                int end = (i == THREAD_COUNT - 1) ? size : (i + 1) * chunk;
                threads.emplace_back(calculate_atom, std::ref(vec), THRESHOLD, start, end);
            }
            for (auto& t : threads) t.join();
        });

        std::cout << size << " " << time_seq << " " << time_mutex << " " << time_atom << std::endl;
    }
    return 0;
}