/*******************************************************************************
 * alx/rolling_hash/reduce_fingerprints.hpp
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

template <typename sss_type>
bool leq_three_tau(uint8_t const* text, size_t text_size, size_t text_pos_i,
                   size_t text_pos_j, sss_type const& sync_set);
template <typename sss_type>
bool eq_three_tau(uint8_t const* text, size_t text_size, size_t text_pos_i,
                  size_t text_pos_j, sss_type const& sync_set);

template <typename sss_type>
std::vector<typename sss_type::index_type> reduce_fps_3tau_lexicographic(
    uint8_t const* text, size_t text_size, sss_type const& sync_set) {
  using index_type = sss_type::index_type;
  static constexpr uint64_t tau = sss_type::tau;

  __extension__ typedef unsigned __int128 uint128_t;
  std::vector<index_type> const& sss = sync_set.get_sss();

  // sort sss-pos by 3tau-infix
  std::vector<index_type> sss_sorted = sss;
  std::sort(sss_sorted.begin(), sss_sorted.end(),
            [&text, &text_size, &sync_set](auto lhs, auto rhs) {
              size_t lce = lce_naive_wordwise<uint8_t>::lce_up_to(
                  text, text_size, lhs, rhs, 3 * tau);
              if (std::max(lhs, rhs) + lce == text_size) {
                return lhs > rhs;
              }
              if (text[lhs + lce] < text[rhs + lce]) {
                return true;
              }
              return sync_set.get_run_info(lhs) < sync_set.get_run_info(rhs);
            });

  for (size_t idx = 1; idx < sss_sorted.size(); ++idx) {
    size_t i = sss_sorted[idx - 1];
    size_t j = sss_sorted[idx];
    assert(leq_three_tau(text, text_size, i, j, sync_set));
  }

  // build tuples
  struct index_rank {
    index_type index;
    index_type rank;
  };

  std::vector<index_rank> rank_tuples(sss_sorted.size());
  index_type cur_rank{1};
  rank_tuples[0] = {sss_sorted[0], cur_rank};
  for (size_t i{1}; i < sss.size(); ++i) {
    if (!eq_three_tau(text, text_size, sss_sorted[i - 1], sss_sorted[i],
                      sync_set)) {
      ++cur_rank;
    }
    rank_tuples[i] = {sss_sorted[i], cur_rank};
  }

  // Check rank_tuples
  {
    assert(rank_tuples.size() == sss_sorted.size());
    for (size_t i = 0; i < rank_tuples.size(); ++i) {
      assert(rank_tuples[i].index == sss_sorted[i]);
    }
    for (size_t i = 1; i < rank_tuples.size(); ++i) {
      bool neq_neighbors = (rank_tuples[i - 1].rank) < (rank_tuples[i].rank);
      assert(neq_neighbors == !eq_three_tau(text, text_size,
                                            rank_tuples[i - 1].index,
                                            rank_tuples[i].index, sync_set));
    }
  }

  // sort tuples by pos
  //[0-3, 1-2, 2-2, 3-0, 4-1, 5-0]
  std::sort(rank_tuples.begin(), rank_tuples.end(),
            [](auto lhs, auto rhs) { return lhs.index < rhs.index; });

  // overwrite
  std::vector<index_type> fps_reduced(sss.size());
  for (size_t i = 0; i < rank_tuples.size(); ++i) {
    fps_reduced[i] = rank_tuples[i].rank;
  }
  return fps_reduced;
}

template <typename sss_type>
bool leq_three_tau(uint8_t const* text, size_t text_size, size_t text_pos_i,
                   size_t text_pos_j, sss_type const& sync_set) {
  constexpr size_t tau = sync_set.tau;
  size_t const max_length = std::min(
      {text_size - text_pos_i, text_size - text_pos_j, 3 * sync_set.tau});
  size_t text_lce = lce_naive_wordwise<uint8_t>::lce_up_to(
      text, text_size, text_pos_i, text_pos_j, 3 * tau);
  return (text_lce < max_length &&
          text[text_pos_i + text_lce] < text[text_pos_j + text_lce]) ||
         (text_lce == max_length && sync_set.get_run_info(text_pos_i) <=
                                        sync_set.get_run_info(text_pos_j));
}

template <typename sss_type>
bool eq_three_tau(uint8_t const* text, size_t text_size, size_t text_pos_i,
                  size_t text_pos_j, sss_type const& sync_set) {
  constexpr size_t tau = sync_set.tau;
  size_t const max_length =
      std::min({text_size - text_pos_i, text_size - text_pos_j, 3 * tau});
  size_t text_lce = lce_naive_wordwise<uint8_t>::lce_up_to(
      text, text_size, text_pos_i, text_pos_j, 3 * tau);

  return ((text_lce == max_length) && sync_set.get_run_info(text_pos_i) ==
                                          sync_set.get_run_info(text_pos_j));
}

}  // namespace alx::lce
/******************************************************************************/
