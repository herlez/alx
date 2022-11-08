/*******************************************************************************
 * alx/lce/lce_sss_noss.hpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once

#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

#include "lce/lce_naive_wordwise.hpp"
#include "pred/pred_index.hpp"
#include "rolling_hash/string_synchronizing_set.hpp"

#ifdef ALX_BENCHMARK_INTERNAL
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "util/timer.hpp"
#ifdef ALX_MEASURE_SPACE
#include <malloc_count/malloc_count.h>
#endif
#endif

namespace alx::lce {

template <typename t_char_type = uint8_t, uint64_t t_tau = 1024,
          typename t_index_type = uint32_t>
class lce_sss_noss {
 public:
  typedef t_char_type char_type;
  __extension__ typedef unsigned __int128 uint128_t;

  lce_sss_noss() : m_text(nullptr), m_size(0) {
  }

  lce_sss_noss(char_type const* text, size_t size)
      : m_text(text), m_size(size) {
    assert(sizeof(t_char_type) == 1);

#ifdef ALX_BENCHMARK_INTERNAL
    alx::util::timer t;
#ifdef ALX_MEASURE_SPACE
    size_t mem_before = malloc_count_current();
    malloc_count_reset_peak();
#endif
#endif

    m_sync_set = rolling_hash::sss<t_index_type, t_tau>(text, size, true);
    // check_string_synchronizing_set(text, m_sync_set);

#ifdef ALX_BENCHMARK_INTERNAL
    fmt::print(" sss_construct_time={}", t.get_and_reset());
    fmt::print(" sss_size={}", m_sync_set.size());
    fmt::print(" sss_runs={}", m_sync_set.num_runs());

#ifdef ALX_MEASURE_SPACE
    fmt::print(" sss_construct_mem={}", malloc_count_current() - mem_before);
    fmt::print(" sss_construct_mem_peak={}", malloc_count_peak() - mem_before);
#endif
#endif

#ifdef ALX_BENCHMARK_INTERNAL
#ifdef ALX_MEASURE_SPACE
    mem_before = malloc_count_current();
    malloc_count_reset_peak();
#endif

#endif
    m_pred = alx::pred::pred_index<t_index_type, 7, t_index_type>(
        m_sync_set.get_sss());

#ifdef ALX_BENCHMARK_INTERNAL
    fmt::print(" pred_construct_time={}", t.get_and_reset());
#ifdef ALX_MEASURE_SPACE
    fmt::print(" pred_construct_mem={}", malloc_count_current() - mem_before);
    fmt::print(" pred_construct_mem_peak={}", malloc_count_peak() - mem_before);
#endif
#endif
  }

  template <typename C>
  lce_sss_noss(C const& container)
      : lce_sss_naive(container.data(), container.size()) {
  }

  // Return the number of common letters in text[i..] and text[j..].
  size_t lce(size_t i, size_t j) const {
    if (i == j) [[unlikely]] {
      assert(i < m_size);
      return m_size - i;
    }
    return lce_uneq(i, j);
  }

  // Return the number of common letters in text[i..] and text[j..]. Here i
  // and j must be different.
  size_t lce_uneq(size_t i, size_t j) const {
    assert(i != j);

    size_t l = std::min(i, j);
    size_t r = std::max(i, j);

    return lce_lr(l, r);
  }

  // Return the number of common letters in text[i..] and text[j..].
  // Here l must be smaller than r.
  inline uint64_t lce_lr(size_t l, size_t r) const {
    // Naive part until synchronizing position

    size_t lce_max{m_size - r};
    size_t lce_local_max{std::min(3 * t_tau, lce_max)};
    size_t lce_local = alx::lce::lce_naive_wordwise<t_char_type>::lce_lr(
        m_text, r + lce_local_max, l, r);

    if (lce_local < lce_local_max || lce_local == lce_max) {
      return lce_local;
    }

    // From synchronizing position
    std::vector<t_index_type> const& sss = m_sync_set.get_sss();
    std::vector<uint128_t> const& fps = m_sync_set.get_fps();

    size_t l_ = m_pred.successor(l).pos;
    size_t r_ = m_pred.successor(r).pos;

    // if(l_-l != r_ -r) {
    //   return
    // }

    size_t block_lce_max = sss.size() - r_;
    size_t block_lce = alx::lce::lce_naive_std<uint128_t>::lce_lr(
        fps.data(), fps.size(), l_, r_);

    size_t l_mm = std::min(sss[l_ + block_lce - 1] + 3 * t_tau, m_size);
    size_t r_mm = std::min(sss[r_ + block_lce - 1] + 3 * t_tau, m_size);
    size_t min_lce = std::min(l_mm - l, r_mm - r);

    size_t final_lce =
        min_lce + alx::lce::lce_naive_wordwise<t_char_type>::lce_lr(
                      m_text, m_size, l + min_lce, r + min_lce);

    // assert(final_lce == alx::lce::lce_naive_wordwise<t_char_type>::lce_lr(
    //                         m_text, m_size, l, r));
    return final_lce;
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
  size_t lce_up_to(size_t i, size_t j, size_t up_to) {
    if (i == j) [[unlikely]] {
      assert(i < m_size);
      return m_size - i;
    }

    size_t l = std::min(i, j);
    size_t r = std::max(i, j);

    // Naive part until synchronizing position
    size_t lce_max{std::min(m_size - r, up_to)};
    size_t lce_local_max{std::min(3 * t_tau, lce_max)};
    size_t lce_local = alx::lce::lce_naive_wordwise<t_char_type>::lce_lr(
        m_text, r + lce_local_max, l, r);

    if (lce_local < lce_local_max || lce_local == lce_max) {
      return lce_local;
    }

    // From synchronizing position
    std::vector<t_index_type> const& sss = m_sync_set.get_sss();
    std::vector<uint128_t> const& fps = m_sync_set.get_fps();

    size_t l_ = m_pred.successor(l).pos;
    size_t r_ = m_pred.successor(r).pos;

    size_t block_lce_max = sss.size() - r_;
    size_t block_lce = alx::lce::lce_naive_std<uint128_t>::lce_lr(
        fps.data(), fps.size(), l_, r_);

    size_t l_mm = sss[l_ + block_lce - 1];
    size_t r_mm = sss[r_ + block_lce - 1];

    size_t lce_rest = alx::lce::lce_naive_wordwise<t_char_type>::lce_lr(
        m_text, m_size, l_mm, r_mm);

    size_t lce = (l_mm - l) + lce_rest;
    return lce;
  }

  char_type operator[](size_t i) {
    return m_text[i];
  }

  size_t size() {
    return m_size;
  }

 private:
 private:
  char_type const* m_text;
  size_t m_size;

  alx::pred::pred_index<t_index_type, 7, t_index_type> m_pred;
  rolling_hash::sss<t_index_type, t_tau> m_sync_set;
  // alx::classic_lce{}
};
}  // namespace alx::lce
/******************************************************************************/
