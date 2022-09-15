/*******************************************************************************
 * alx/pred/binsearch_std.hpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once

#include <assert.h>

#include <algorithm>
#include <cstddef>
#include <iterator>

#include "pred_result.hpp"

namespace alx::pred {
template <typename t_data_type>
class binsearch_std {
 public:
  typedef t_data_type data_type;
  binsearch_std() : m_data(nullptr), m_size(0), m_min(0), m_max(0) {
  }

  binsearch_std(t_data_type const* data, size_t size)
      : m_data(data), m_size(size) {
    assert(std::is_sorted(data, data + size));
    if (m_size != 0) {
      m_min = data[0];
      m_max = data[size - 1];
    }
  }

  template <typename C>
  binsearch_std(C const& container)
      : binsearch_std(container.data(), container.size()) {
  }

  template <bool t_true_predecessor = false>
  result predecessor(t_data_type x) const {
    if (x < m_min) {
      return result{false, 0};
    }
    if (t_true_predecessor && x == m_min) {
      return result{false, 0};
    }
    return result{true, predecessor_unsafe<t_true_predecessor>(x)};
  }

  template <bool t_true_predecessor = false>
  size_t predecessor_unsafe(t_data_type x) const {
    if constexpr (t_true_predecessor) {
      assert(x > m_min);
      return std::distance(m_data, std::lower_bound(m_data, m_data + m_size, x)) - 1;
    } else {
      assert(x >= m_min);
      return std::distance(m_data, std::upper_bound(m_data, m_data + m_size, x)) - 1;
    }
  }

  template <bool t_true_successor = false>
  result successor(t_data_type x) const {
    if (x > m_max) {
      return result{false, 0};
    }
    if (t_true_successor && x == m_max) {
      return result{false, 0};
    }
    return result{true, successor_unsafe<t_true_successor>(x)};
  }

  template <bool t_true_successor = false>
  size_t successor_unsafe(t_data_type x) const {
    if constexpr (t_true_successor) {
      assert(x < m_max);
      return std::distance(m_data, std::upper_bound(m_data, m_data + m_size, x));
    } else {
      assert(x <= m_max);
      return std::distance(m_data, std::lower_bound(m_data, m_data + m_size, x));
    }
  }

  bool contains(t_data_type x) const {
    auto const it = std::lower_bound(m_data, m_data + m_size, x);
    return (it != m_data + m_size) && (*it == x);
  }

 private:
  t_data_type const* m_data = nullptr;
  size_t m_size = 0;
  t_data_type m_min = 0;
  t_data_type m_max = 0;
};

};  // namespace alx::pred