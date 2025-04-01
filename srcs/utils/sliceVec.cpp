#include <vector>

/**
 * @brief Creates a copy of vec in the range of [start_idx, end_idx]
 *
 * @param vec vector to slice
 * @return slice of original vec
 */
template <typename T>
std::vector<T> sliceVec(std::vector<T>& vec, unsigned int start_idx, unsigned int end_idx) {
    typename std::vector<T>::iterator start_itr = vec.begin() + start_idx;
    typename std::vector<T>::iterator end_itr = vec.begin() + end_idx + 1;

    typename std::vector<T> result_vec(end_idx - start_idx + 1);
    std::copy(start_itr, end_itr, result_vec.begin());

    return result_vec;
}
