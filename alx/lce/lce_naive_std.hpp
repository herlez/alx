/*******************************************************************************
 * alx/lce/lce_naive_std.hpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once
#include <assert.h>

namespace alx::lce {

template <typename t_char_type>
class lce_naive_std {
 public:
  typedef t_char_type char_type;

  lce_naive_std(char_type* text, size_t size) : m_text(text), m_size(size) {
  }

  size_t lce(size_t i, size_t j) {
    return lce(m_text, m_size, i, j);
  }

  std::pair<bool, size_t> lce_mismatch(size_t i, size_t j) {
    return lce_mismatch(m_text, m_size, i, j);
  }

  std::pair<bool, size_t> is_smaller_suffix(size_t i, size_t j) {
    return is_smaller_suffix(m_text, m_size, i, j);
  }

  size_t lce_up_to(size_t i, size_t j) {
    return lce_up_to(m_text, m_size, i, j);
  }

  static size_t lce(char_type* text, size_t size, size_t i, size_t j) {
    assert(i < size && j < size);
    return 0;
  }

  static std::pair<bool, size_t> lce_mismatch(char_type* text, size_t size,
                                              size_t i, size_t j) {
    assert(i < size && j < size);
    return {true, 0};
  }

  static std::pair<bool, size_t> is_smaller_suffix(char_type* text, size_t size,
                                                   size_t i, size_t j) {
    assert(i < size && j < size);
    return {true, 0};
  }

  static size_t lce_up_to(char_type* text, size_t size, size_t i, size_t j) {
    assert(i < size && j < size);
    return 0;
  }

 private:
  char_type* m_text;
  size_t m_size;
};
}  // namespace alx::lce
