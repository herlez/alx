/*******************************************************************************
 * alx/lce/lce_classic.hpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once
#include <assert.h>
#include <libsais.h>
#include <libsais64.h>

#include <cstdint>
#include <gsaca-double-sort-par.hpp>

#include "lce/lce_naive_wordwise.hpp"
#include "rmq/rmq_nlgn.hpp"

#ifdef ALX_BENCHMARK_INTERNAL
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "util/timer.hpp"
#ifdef ALX_MEASURE_SPACE
#include <malloc_count/malloc_count.h>
#endif
#endif

namespace alx::lce {

template <typename t_char_type = uint8_t, typename t_index_type = uint32_t>
class lce_classic {
 public:
  typedef t_char_type char_type;

  lce_classic() : m_text(nullptr), m_size(0) {
  }

  lce_classic(char_type const* text, size_t size) : m_text(text), m_size(size) {
    std::vector<t_index_type> sa(size);
    // sort sa
#ifdef ALX_BENCHMARK_INTERNAL
    alx::util::timer t;
#ifdef ALX_MEASURE_SPACE
    size_t mem_before = malloc_count_current();
    malloc_count_reset_peak();
#endif
#endif

    if constexpr (sizeof(t_index_type) == 4 && sizeof(t_char_type) == 1) {
      libsais(reinterpret_cast<const uint8_t*>(text),
              reinterpret_cast<int32_t*>(sa.data()), size, 0, nullptr);
    } else if constexpr (sizeof(t_index_type) == 8 &&
                         sizeof(t_char_type) == 1) {
      libsais64(reinterpret_cast<const uint8_t*>(text),
                reinterpret_cast<int64_t*>(sa.data()), size, 0, nullptr);
    } else {
      gsaca_ds1_par(text, sa.data(), size);
    }
#ifdef ALX_BENCHMARK_INTERNAL
    std::cout << " gsaca_time=" << t.get_and_reset();
#endif

#ifdef ALX_BENCHMARK_INTERNAL
    fmt::print(" gsaca_time={}", t.get_and_reset());
#ifdef ALX_MEASURE_SPACE
    fmt::print(" gsaca_mem={}", malloc_count_current() - mem_before);
    fmt::print(" gsaca_mem_peak={}", malloc_count_peak() - mem_before);
#endif
#endif

    // build isa
    m_isa.resize(size);
#pragma omp parallel for
    for (size_t i = 0; i < sa.size(); ++i) {
      m_isa[sa[i]] = i;
    }

    // build lcp
    m_lcp.resize(sa.size());
    m_lcp[0] = 0;
    size_t current_lcp = 0;
#pragma omp parallel for firstprivate(current_lcp)
    for (size_t i = 0; i < m_lcp.size() - 1; ++i) {
      size_t suffix_array_pos = m_isa[i];
      assert(suffix_array_pos != 0);

      size_t preceding_suffix_pos = sa[suffix_array_pos - 1];
      current_lcp += lce_naive_wordwise<char_type>::lce_uneq(
          text, size, i + current_lcp, preceding_suffix_pos + current_lcp);
      m_lcp[suffix_array_pos] = current_lcp;
      if (current_lcp != 0) {
        --current_lcp;
      }
    }
    // built rmq
    m_rmq = alx::rmq::rmq_nlgn<t_index_type>(m_lcp);
  }

  template <typename C>
  lce_classic(C const& container)
      : lce_classic(container.data(), container.size()) {
  }

  // Return the number of common letters in text[i..] and text[j..].
  size_t lce(size_t i, size_t j) const {
    if (i == j) [[unlikely]] {
      assert(i < m_size);
      return m_size - i;
    }
    return lce_uneq(i, j);
  }

  // Return the number of common letters in text[i..] and text[j..]. Here i and
  // j must be different.
  size_t lce_uneq(size_t i, size_t j) const {
    assert(i != j);
    return lce_lr(i, j);
  }

  // Return the number of common letters in text[i..] and text[j..].
  // Here l must be smaller than r.
  size_t lce_lr(size_t l, size_t r) const {
    return m_lcp[m_rmq.rmq_shifted(m_isa[l], m_isa[r])];
  }

  // Return {b, lce}, where lce is the number of common letters in text[i..]
  // and text[j..] and b tells whether the lce ends with a mismatch.
  std::pair<bool, size_t> lce_mismatch(size_t i, size_t j) {
    if (i == j) [[unlikely]] {
      assert(i < m_size);
      return {false, m_size - i};
    }
    size_t l = std::min(i, j);
    size_t r = std::max(i, j);

    size_t lce = lce_lr(l, r);
    return {r + lce != m_size, lce};
  }

  // Return whether text[i..] is lexicographic smaller than text[j..]. Here i
  // and j must be different.
  bool is_leq_suffix(size_t i, size_t j) {
    assert(i != j);
    size_t lce_val = lce_uneq(i, j);
    return (
        i + lce_val == m_size ||
        ((j + lce_val != m_size) && m_text[i + lce_val] < m_text[j + lce_val]));
  }

  // Return the lce of text[i..i+lce) and text[j..j+lce]
  size_t lce_up_to(size_t i, size_t j, size_t up_to) const {
    size_t lce_val = lce_uneq(i, j);
    return std::min(up_to, lce_val);
  }

 private:
  std::vector<t_index_type> m_isa;
  std::vector<t_index_type> m_lcp;
  // alx::rmq_nlgn m_lcp;

  char_type const* m_text;
  size_t m_size;
  alx::rmq::rmq_nlgn<t_index_type> m_rmq;
};

}  // namespace alx::lce
