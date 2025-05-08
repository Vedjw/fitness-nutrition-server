#ifndef RANDOM_MT_H
#define RANDOM_MT_H

#include <chrono>
#include <random>


namespace Random
{

    inline std::mt19937 generate()
    {
        std::random_device rd{};

        // Create seed_seq with clock and 7 random numbers from std::random_device
        std::seed_seq ss{
            static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
                rd(), rd(), rd(), rd(), rd(), rd(), rd() };

        return std::mt19937{ ss };
    }

    // Here's our global std::mt19937 object.
    // The inline keyword means we only have one global instance for our whole program.
    inline std::mt19937 mt{ generate() }; // generates a seeded std::mt19937 and copies it into our global object

    // Generate a random int between [min, max] (inclusive)
        // * also handles cases where the two arguments have different types but can be converted to int
    inline int get(int min, int max)
    {
        return std::uniform_int_distribution{ min, max }(mt);
    }

    template <typename T>
    T get(T min, T max)
    {
        return std::uniform_int_distribution<T>{min, max}(mt);
    }

    // Generate a random value between [min, max] (inclusive)
    // * min and max can have different types
        // * return type must be explicitly specified as a template argument
    // * min and max will be converted to the return type
    // Sample call: Random::get<std::size_t>(0, 6);  // returns std::size_t
    // Sample call: Random::get<std::size_t>(0, 6u); // returns std::size_t
    // Sample call: Random::get<std::int>(0, 6u);    // returns int
    template <typename R, typename S, typename T>
    R get(S min, T max)
    {
        return get<R>(static_cast<R>(min), static_cast<R>(max));
    }
}

#endif