#include <cstdint>
#include <iomanip>
#include "utils.hpp"

void test_random_scalars(){
    std::cout << std::setprecision(5);
    std::cout << "test_random_scalars:" << std::endl;
    Random rng;
    // bool
    std::cout << "random_bool(): " << rng.random_bool() << std::endl;
    // int
    std::cout << "randint<int>(): " << rng.randint<int>() << std::endl;
    std::cout << "randint<int>(-10, 10): " << rng.randint<int>(-10, 10) << std::endl;
    std::cout << "randint<long>(): " << rng.randint<long>() << std::endl;
    std::cout << "randint<long long>(): " << rng.randint<long long>() << std::endl;
    std::cout << "randint<int8_t>(): " << (int)rng.randint<int8_t>() << std::endl;
    std::cout << "randint<int16_t>(): " << (int)rng.randint<int16_t>() << std::endl;
    std::cout << "randint<int32_t>(): " << rng.randint<int32_t>() << std::endl;
    std::cout << "randint<int64_t>(): " << rng.randint<int64_t>() << std::endl;
    std::cout << "randint<uint8_t>(): " << (int)rng.randint<uint8_t>() << std::endl;
    std::cout << "randint<uint16_t>(): " << (int)rng.randint<uint16_t>() << std::endl;
    std::cout << "randint<uint32_t>(): " << rng.randint<uint32_t>() << std::endl;
    std::cout << "randint<uint64_t>(): " << rng.randint<uint64_t>() << std::endl;
    // float
    std::cout << "uniform<float>(): " << rng.uniform<float>() << std::endl;
    std::cout << "uniform<float>(-10.0, 10.0): " << rng.uniform<float>(-10.0f, 10.0f) << std::endl;
    std::cout << "uniform<double>(): " << rng.uniform<double>() << std::endl;
    std::cout << "uniform<double>(-10.0, 10.0): " << rng.uniform<double>(-10.0, 10.0) << std::endl;
    std::cout << "uniform<long double>(): " << rng.uniform<long double>() << std::endl;
    // string
    std::cout << "random_string(): " << rng.random_string(CHARSET_ALPHANUMERIC, 10) << std::endl;
    std::cout << std::endl;
}

void test_random_vector(){
    std::cout << std::setprecision(5);
    std::cout << "test_random_vector:" << std::endl;
    Random rng; size_t n = 10;
    // bool
    std::vector<bool> vec_bool(n);
    rng.initialize_vector_with_random_values(vec_bool, true);
    std::cout << "Random vector of booleans: "; MyPrint(vec_bool);
    std::vector<uint8_t> vec_bool_uint8(n);
    rng.initialize_vector_with_random_values(vec_bool_uint8, true);
    std::cout << "Random vector of booleans (in uint8): "; MyPrint(vec_bool_uint8);
    // int
    std::vector<int> vec_int(n);
    rng.initialize_vector_with_random_values(vec_int);
    std::cout << "int: "; MyPrint(vec_int);
    std::vector<int8_t> vec_int8(n);
    rng.initialize_vector_with_random_values(vec_int8);
    std::cout << "int8_t: "; MyPrint(vec_int8);
    std::vector<int16_t> vec_int16(n);
    rng.initialize_vector_with_random_values(vec_int16);
    std::cout << "int16_t: "; MyPrint(vec_int16);
    std::vector<int32_t> vec_int32(n);
    rng.initialize_vector_with_random_values(vec_int32);
    std::cout << "int32_t: "; MyPrint(vec_int32);
    std::vector<int64_t> vec_int64(n);
    rng.initialize_vector_with_random_values(vec_int64);
    std::cout << "int64_t: "; MyPrint(vec_int64);
    // uint
    std::vector<uint8_t> vec_uint8(n);
    rng.initialize_vector_with_random_values(vec_uint8);
    std::cout << "uint8: "; MyPrint(vec_uint8);
    std::vector<uint16_t> vec_uint16(n);
    rng.initialize_vector_with_random_values(vec_uint16);
    std::cout << "uint16: "; MyPrint(vec_uint16);
    std::vector<uint32_t> vec_uint32(n);
    rng.initialize_vector_with_random_values(vec_uint32);
    std::cout << "uint32: "; MyPrint(vec_uint32);
    std::vector<uint64_t> vec_uint64(n);
    rng.initialize_vector_with_random_values(vec_uint64);
    std::cout << "uint64: "; MyPrint(vec_uint64);
    // float
    std::vector<float> vec_float(n);
    rng.initialize_vector_with_random_values(vec_float);
    std::cout << "Random vector of floats: "; MyPrint(vec_float);
    // double
    std::vector<double> vec_double(n);
    rng.initialize_vector_with_random_values(vec_double);
    std::cout << "Random vector of doubles: "; MyPrint(vec_double);
    std::vector<int> vec(10);
    rng.initialize_vector_with_random_values(vec);

    std::cout << std::endl;
}

int main(){
    test_random_scalars();
    test_random_vector();
    return 0;
}