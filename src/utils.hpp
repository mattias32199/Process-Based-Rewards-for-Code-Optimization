#ifndef UTILS_HPP
#define UTILS_HPP

#include <immintrin.h>
#include <x86intrin.h>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>
#include <limits>
#include <type_traits>
#include <cstring>
#include <numeric> // Required for std::iota

// --- MACRO GUARDS ---
#ifndef ITERATIONS
#define ITERATIONS 100
#endif

#ifndef Small_Arg_1D
#define Small_Arg_1D 128
#endif
#ifndef Small_Arg_2D
#define Small_Arg_2D 32
#endif

#ifndef ERROR_PRINT
#define ERROR_PRINT true
#endif

#ifndef DEFAULT_SEED
#define DEFAULT_SEED 12345
#endif
// --------------------

class Random {
private:
    std::mt19937_64 gen;
    std::uniform_int_distribution<int64_t> int_dist;
    std::uniform_real_distribution<double> real_dist;

public:
    // 1. Default Constructor (Fixed Seed 42)
    // CRITICAL: Must be fixed seed so SimdBench tasks without explicit seeds are deterministic.
    Random() : gen(42),
               int_dist(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()),
               real_dist(0.0, 1.0) {}

    // 2. Seeded Constructor (For TSVC)
    Random(int seed) : gen(seed),
               int_dist(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()),
               real_dist(0.0, 1.0) {}

    // --- BASIC HELPERS ---
    bool random_bool() {
        std::uniform_int_distribution<> dist(0, 1);
        return dist(gen) == 1;
    }

    template<typename T>
    T random() {
        return static_cast<T>(real_dist(gen));
    }

    template<typename T>
    T uniform(T a = T(-100.0), T b = T(100.0)) {
        if constexpr (std::is_floating_point_v<T>) {
            if (a > b) std::swap(a, b);
            std::uniform_real_distribution<T> dist(a, b);
            return dist(gen);
        } else {
            if (a > b) std::swap(a, b);
            std::uniform_int_distribution<T> dist(a, b);
            return dist(gen);
        }
    }

    template<typename T>
    T randint(T min, T max) {
        if constexpr (std::is_integral_v<T>) {
            if(min > max) std::swap(min, max);
            std::uniform_int_distribution<T> dis(min, max);
            return dis(gen);
        } else {
            if(min > max) std::swap(min, max);
            std::uniform_real_distribution<T> dis(min, max);
            return dis(gen);
        }
    }

    // --- MISSING HELPERS (Added for Sorting/Search Tasks) ---

    // Equivalent to random.shuffle
    template<typename T>
    void shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), gen);
    }

    // Equivalent to random.choice
    template<typename T>
    T choice(const std::vector<T>& vec) {
        if(vec.empty()) return T{};
        std::uniform_int_distribution<> dist(0, vec.size() - 1);
        return vec[dist(gen)];
    }

    // Used by s2711 and other sorting benchmarks
    void initialize_vector_with_permutation(std::vector<int>& vec) {
        std::iota(vec.begin(), vec.end(), 1); // Fill 1, 2, 3...
        std::shuffle(vec.begin(), vec.end(), gen);
    }
    // --------------------------------------------------------

    template<typename T>
    void initialize_vector_with_random_values(std::vector<T>& vec, bool binary = false) {
        if constexpr (std::is_integral_v<T>) {
            if (binary) {
                for (auto& val : vec) val = static_cast<T>(gen() % 2);
            } else {
                for (auto& val : vec) val = static_cast<T>(int_dist(gen));
            }
        } else {
            for (auto& val : vec) val = static_cast<T>(real_dist(gen));
        }
    }
};

// 1. Vector allclose
template<typename T>
bool allclose(const std::vector<T>& a, const std::vector<T>& b, double rtol = 1e-5, double atol = 1e-8) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if constexpr (std::is_floating_point_v<T>) {
            if (std::isnan(a[i]) && std::isnan(b[i])) continue;
            if (std::isnan(a[i]) || std::isnan(b[i])) return false;
            double diff = std::abs(static_cast<double>(a[i]) - static_cast<double>(b[i]));
            double threshold = atol + rtol * std::abs(static_cast<double>(b[i]));
            if (diff > threshold) {
                if (ERROR_PRINT) std::cerr << "Mismatch at index " << i << ": " << a[i] << " vs " << b[i] << std::endl;
                return false;
            }
        } else {
            if (a[i] != b[i]) {
                if (ERROR_PRINT) std::cerr << "Mismatch at index " << i << ": " << a[i] << " vs " << b[i] << std::endl;
                return false;
            }
        }
    }
    return true;
}

// 2. Scalar allclose
template<typename T>
bool allclose(const T& a, const T& b, double rtol = 1e-5, double atol = 1e-8) {
    if constexpr (std::is_floating_point_v<T>) {
        if (std::isnan(a) && std::isnan(b)) return true;
        if (std::isnan(a) || std::isnan(b)) return false;
        double diff = std::abs(static_cast<double>(a) - static_cast<double>(b));
        double threshold = atol + rtol * std::abs(static_cast<double>(b));
        return diff <= threshold;
    } else {
        return a == b;
    }
}

#endif // UTILS_HPP
