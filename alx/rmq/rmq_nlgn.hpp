/*******************************************************************************
 * alx/rmq/rmq_nlgn.hpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once

#include <assert.h>
#include <omp.h>

#include <vector>

namespace alx::rmq {
inline size_t log2_of_uint32(uint32_t const x) {
  assert(x != 0);
  return 31 - __builtin_clz(x);
}

template <typename key_type>
class rmq_nlgn {
  key_type const* m_data = nullptr;
  std::vector<std::vector<uint32_t>> m_power_rmq;

 public:
  rmq_nlgn() {
  }

  rmq_nlgn(key_type const* data, size_t size) : m_data(data) {
    const uint32_t m_num_levels = log2_of_uint32(size);
    m_power_rmq.resize(m_num_levels);

    // Build first level
    m_power_rmq[0].resize(size - 1);

#pragma omp parallel for
    for (size_t i = 0; i < size - 1; ++i) {
      m_power_rmq[0][i] = m_data[i] < data[i + 1] ? i : (i + 1);
    }

    // Build the rest
    for (size_t l = 1; l < m_num_levels; ++l) {
      m_power_rmq[l].resize(size - ((uint64_t{2} << l) - 1));
      uint32_t const span = (uint64_t{1} << l);
#pragma omp parallel for
      for (size_t i = 0; i < m_power_rmq[l].size(); ++i) {
        const uint32_t l_interval_min = m_power_rmq[l - 1][i];
        const uint32_t r_interval_min = m_power_rmq[l - 1][i + span];
        m_power_rmq[l][i] = m_data[l_interval_min] < m_data[r_interval_min]
                                ? l_interval_min
                                : r_interval_min;
      }
    }
  }

  template <typename C>
  rmq_nlgn(C const& container) : rmq_nlgn(container.data(), container.size()) {
  }

  size_t rmq(size_t const i, size_t const j) const {
    size_t const left = std::min(i, j);
    size_t const right = std::max(i, j);
    const uint32_t dist = right - left;
    if (dist <= 1) {
      return m_data[left] <= m_data[right] ? left : right;
    }

    const uint32_t dist_log = log2_of_uint32(dist);
    const uint32_t max_power_span = (1ULL << dist_log);
    const uint32_t l_interval_min = m_power_rmq[dist_log - 1][left];
    const uint32_t r_interval_min =
        m_power_rmq[dist_log - 1][right + 1 - max_power_span];

    return m_data[l_interval_min] < m_data[r_interval_min] ? l_interval_min
                                                           : r_interval_min;
  }

  size_t rmq_shifted(size_t const i, size_t const j) const {
    assert(i != j);
    size_t const left = std::min(i, j) + 1;
    size_t const right = std::max(i, j);
    const uint32_t dist = right - left;
    if (dist <= 1) {
      return m_data[left] <= m_data[right] ? left : right;
    }

    const uint32_t dist_log = log2_of_uint32(dist);
    const uint32_t max_power_span = (1ULL << dist_log);
    const uint32_t l_interval_min = m_power_rmq[dist_log - 1][left];
    const uint32_t r_interval_min =
        m_power_rmq[dist_log - 1][right + 1 - max_power_span];

    return m_data[l_interval_min] < m_data[r_interval_min] ? l_interval_min
                                                           : r_interval_min;
  }

};  // class rmq_nlgn
}  // namespace alx::rmq