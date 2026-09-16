#include "../headers/UnqPtr.h"
#include "../headers/ShrdPtr.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

struct TestResult {
    std::string variant;
    size_t n;
    double timeMs;
    size_t wrapperOverheadBytes;
    size_t totalOverheadBytes;
};

// Прогрев аллокатора: заставляет ОС выделить процессу страницы памяти
// заранее, до начала замеров, чтобы первый измеряемый вариант не "платил"
// за это в одиночку и не искажал сравнение.
static void WarmUpAllocator() {
    std::vector<int*> warm;
    warm.reserve(20000);
    for (int i = 0; i < 20000; ++i) {
        warm.push_back(new int(i));
    }
    for (int* p : warm) {
        delete p;
    }
}

static double MeasureRaw(size_t n) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int*> raw;
    raw.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        raw.push_back(new int(static_cast<int>(i)));
    }
    for (int* p : raw) {
        delete p;
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

static double MeasureUnqPtr(size_t n) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<UnqPtr<int>> ptrs;
    ptrs.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        ptrs.push_back(UnqPtr<int>(new int(static_cast<int>(i))));
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

static double MeasureStdUniquePtr(size_t n) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::unique_ptr<int>> ptrs;
    ptrs.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        ptrs.push_back(std::make_unique<int>(static_cast<int>(i)));
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// Прогоняет измерение несколько раз подряд и возвращает медиану —
// устойчивее к случайным выбросам (фоновые процессы ОС, кэш и т.п.),
// чем среднее арифметическое или единичный замер.
static double MedianOf(std::vector<double> samples) {
    std::sort(samples.begin(), samples.end());
    size_t mid = samples.size() / 2;
    if (samples.size() % 2 == 0) {
        return (samples[mid - 1] + samples[mid]) / 2.0;
    }
    return samples[mid];
}

static const int kRepeats = 5; // сколько раз повторяем каждый замер

static TestResult BenchRawPointers(size_t n) {
    std::vector<double> samples;
    for (int i = 0; i < kRepeats; ++i) {
        samples.push_back(MeasureRaw(n));
    }
    size_t wrapperSize = sizeof(int*);
    return {"raw new/delete", n, MedianOf(samples), wrapperSize, wrapperSize * n};
}

static TestResult BenchUnqPtr(size_t n) {
    std::vector<double> samples;
    for (int i = 0; i < kRepeats; ++i) {
        samples.push_back(MeasureUnqPtr(n));
    }
    size_t wrapperSize = sizeof(UnqPtr<int>);
    return {"UnqPtr", n, MedianOf(samples), wrapperSize, wrapperSize * n};
}

static TestResult BenchStdUniquePtr(size_t n) {
    std::vector<double> samples;
    for (int i = 0; i < kRepeats; ++i) {
        samples.push_back(MeasureStdUniquePtr(n));
    }
    size_t wrapperSize = sizeof(std::unique_ptr<int>);
    return {"std::unique_ptr", n, MedianOf(samples), wrapperSize, wrapperSize * n};
}

static void PrintTable(const std::vector<TestResult>& results) {
    std::cout << "\nN\t\tVariant\t\t\tTime(ms, median of " << kRepeats << ")\tWrapperSize(B)\tTotalOverhead(B)\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    for (const auto& r : results) {
        std::cout << r.n << "\t\t" << r.variant << "\t\t"
                  << r.timeMs << "\t\t\t" << r.wrapperOverheadBytes
                  << "\t\t" << r.totalOverheadBytes << "\n";
    }
}

static void SaveCsv(const std::vector<TestResult>& results, const std::string& path) {
    std::ofstream out(path);
    out << "n,variant,time_ms,wrapper_size_bytes,total_overhead_bytes\n";
    for (const auto& r : results) {
        out << r.n << "," << r.variant << "," << r.timeMs << ","
            << r.wrapperOverheadBytes << "," << r.totalOverheadBytes << "\n";
    }
    std::cout << "\nResults saved to " << path << "\n";
}

void RunLoadTests() {
    std::vector<size_t> sizes = {
            10, 100, 1000,
            100000, 1000000, 10000000
    };

    std::cout << "Warming up allocator...\n";
    WarmUpAllocator();

    std::vector<TestResult> results;

    std::cout << "Running load tests (median of " << kRepeats << " runs each), this may take a while...\n";
    for (size_t n : sizes) {
        results.push_back(BenchRawPointers(n));
        results.push_back(BenchUnqPtr(n));
        results.push_back(BenchStdUniquePtr(n));
    }

    PrintTable(results);
    SaveCsv(results, "load_test_results.csv");
}