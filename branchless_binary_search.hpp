// This version outputs a `cmovg` instruction using GCC.

#pragma once

#include <bit>
#include <functional>
#include <stdint.h>

inline size_t bit_floor(size_t i) {
  constexpr int num_bits = sizeof(i) * 8;
  return size_t(1) << (num_bits - std::__countl_zero(i) - 1);
}

inline size_t bit_ceil(size_t i) {
  constexpr int num_bits = sizeof(i) * 8;
  return size_t(1) << (num_bits - std::__countl_zero(i - 1));
}

template <typename It, typename T, typename Cmp>
It branchless_lower_bound(It begin, It end, const T &value, Cmp &&compare) {
  std::size_t length = end - begin;
  if (length == 0)
    return end;

  std::size_t step = bit_floor(length);

  if (step != length && compare(begin[step], value)) {
    length -= step + 1;
    if (length == 0)
      return end;
    step = bit_ceil(length);
    begin = end - step;
  }

  for (step /= 2; step != 0; step /= 2) {
    It candidate = begin + step;
    if (compare(*candidate, value))
      begin = candidate;
  }

  return begin + compare(*begin, value);
}

template <typename It, typename T>
It branchless_lower_bound(It begin, It end, const T &value) {
  return branchless_lower_bound(begin, end, value, std::less<>{});
}
