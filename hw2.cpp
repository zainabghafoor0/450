// HW2: Sequential vs Concurrent Threads Timing
// Build: g++ -O2 -std=c++17 hw2.cpp -o hw2
// Run:   ./hw2

#include <iostream>
#include <fstream>
#include <thread>
#include <cmath>
#include <chrono>
#include <vector>
#include <atomic>
#include <algorithm>

using namespace std;

// -----------------------------
// Output file
// -----------------------------
ofstream result("./result", ios::out);

// -----------------------------
// IMPORTANT: prevent optimization
// -----------------------------
static volatile long long g_sink_int = 0;
static volatile double    g_sink_dbl = 0.0;

// -----------------------------
// Choose function
// -----------------------------
static constexpr bool USE_FUNC2 = false;

// -----------------------------
// Workload functions (with side-effects)
// -----------------------------

void func_1() {
    long long a = 13;
    long long b = 5;
    long long c = 0;

    for (int i = 1; i <= 10000000; i++) {
        a = a * b;
        c = a + b;
    }

    // Side-effect so compiler cannot remove loop
    g_sink_int += c;
}

void func_2() {
    double a = 12.56789;
    double b = 7.141565758;
    double c = 0.0;

    for (int i = 1; i <= 100000000; i++) {
        c = sin(a * 35.789 / b);
        a = cbrt(c);
    }

    // Side-effect so compiler cannot remove loop
    g_sink_dbl += a;
}

inline void run_selected() {
    if constexpr (USE_FUNC2) func_2();
    else func_1();
}

// -----------------------------
// Timing helpers (milliseconds)
// -----------------------------
double time_sequential(int n_calls) {
    auto start = chrono::steady_clock::now();
    for (int i = 0; i < n_calls; i++) run_selected();
    auto end = chrono::steady_clock::now();
    chrono::duration<double, milli> dt = end - start;
    return dt.count();
}

double time_threads(int n_threads) {
    auto start = chrono::steady_clock::now();

    vector<thread> threads;
    threads.reserve(n_threads);

    for (int i = 0; i < n_threads; i++) {
        threads.emplace_back(run_selected);
    }
    for (auto &t : threads) t.join();

    auto end = chrono::steady_clock::now();
    chrono::duration<double, milli> dt = end - start;
    return dt.count();
}

// -----------------------------
// Main comparison
// -----------------------------
void compare() {
    const unsigned lp = thread::hardware_concurrency();

    // Assignment sample expects 1-20.
    // Also avoids crazy output on servers with 64/128+ logical processors.
    const int max_iterations = 20;

    result << "Logical Processors (C++ hw_concurrency): " << lp << "\n";
    result << "Max iterations (1.." << max_iterations << ")\n";
    result << "Function under test: " << (USE_FUNC2 ? "func_2()" : "func_1()") << "\n\n";

    // Optional warm-up (reduces first-iteration noise)
    run_selected();

    // Sequential
    result << "--------- Run function " << (USE_FUNC2 ? "func_2()" : "func_1()")
           << " 1-" << max_iterations << " times sequentially\n";

    for (int i = 1; i <= max_iterations; i++) {
        double ms = time_sequential(i);
        result << i << " " << ms << "\n";
    }

    // Threads
    result << "\n--------- Run function " << (USE_FUNC2 ? "func_2()" : "func_1()")
           << " as 1-" << max_iterations << " threads concurrently\n";

    for (int i = 1; i <= max_iterations; i++) {
        double ms = time_threads(i);
        result << i << " " << ms << "\n";
    }

    result << "\nDone.\n";
}

int main() {
    compare();
    return 0;
}
