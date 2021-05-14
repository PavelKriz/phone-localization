//----------------------------------------------------------------------------------------
/**
 * \file       SpecializedInputOutput.h
 * \author     Pavel Kriz
 * \date       10/5/2021
 * \brief      Contains functions that has some general purpose in the input or output logics 
 *
 * (all of them are in sio namespace - specialized input output)
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

#include <limits>
#include <type_traits>
#include <string>

using namespace std;

const float compA = std::numeric_limits<float>::min();

namespace sio {
    /**
     * @brief Check whether is the range in positive range (works for floating and whole numbers) - 0 is considered positive
     * 
     * For floating points is considered the inacuracy of floats (for doubles too)
     * 
     * @tparam T number type that is one of the basic C++ types (with objects it might not work as well)
     * @param x the number
     * @return if it is positive or not
    */
    template<typename T>
    bool numberInPositiveRange(const T& x) {
        if (is_floating_point<T>::value) {
            return (x + compA) >= 0;
        }
        else {
            return x >= 0;
        }
    }
    /**
     * @brief Check whether is the number in given range (works for floating and whole numbers) - including the range limits
     * 
     * For floating points is considered the inacuracy of floats (for doubles too)
     * 
     * @tparam T T number type that is one of the basic C++ types (with objects it might not work as well)
     * @param x the number
     * @param min range start
     * @param max range end
     * @return whether is the number in range
    */
    template<typename T>
    bool numberInRange(const T& x, const T& min, const T& max) {
        if (is_floating_point<T>::value) {
            if ((x - compA) <= max && (x + compA) >= min) {
                return true;
            }
            return false;
        }
        else {
            if (x <= max && x >= min) {
                return true;
            }
            return false;
        }
    }
    /**
     * @brief Removes the last suffix of a filapath/filename: "/filepath/file.suffix" => "/filepath/file"
     * @param filepath to be editet
     * @return the edited filepath/filename
    */
    string getFilePathWithoutSuffix(const string& filepath);
}