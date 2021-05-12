//----------------------------------------------------------------------------------------
/**
 * \file       SpecializedInputOutput.h
 * \author     Pavel Kriz
 * \date       10/5/2021
 * \brief      Contains functions that has some general purpose in the input or output logics
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

    template<typename T>
    bool numberInPositiveRange(const T& x) {
        if (is_floating_point<T>::value) {
            return (x + compA) >= 0;
        }
        else {
            return x >= 0;
        }
    }

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

    string getFilePathWithoutSuffix(const string& filepath);
}