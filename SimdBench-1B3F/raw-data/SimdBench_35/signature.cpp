/*
The inputs to this function are::
- A pointer to a non-empty 16-bit integer vector (vec);
- The length of the given vector.

Your goal is to return the the maximum and minimum values in the given vector.
The function should return a value of the defined `MinMaxPair` type:
```c
typedef struct {
    int16_t min_num;
    int16_t max_num;
} MinMaxPair;
```
The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
MinMaxPair min_max_pair_simd(const int16_t* vec, size_t length) {
}