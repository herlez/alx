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

#include "rolling_hash/string_synchronizing_set.hpp"
#include "lce/lce_naive_wordwise.hpp"


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
std::vector<typename sss_type::index_type> reduce_fps_3tau_lexicographic(
    uint8_t const* text, size_t text_size, sss_type const& sss) {
  using index_type = sss_type::index_type;
  // sort sss-pos by 3tau-infix
  std::vector<index_type> sss_sorted = sss.get_sss();
  //std::sort(sss_sorted.begin(), sss_sorted.end(), []() {});
  // sort

  // build tuples
  //[5-0, 3-0, 4-1, 2-2, 1-2, 0-3]
  struct index_rank {
    index_type index;
    index_type rank;
  };

  std::vector<index_rank> rank_tuples(sss_sorted.size());
  index_type cur_rank{1};
  for (size_t i{1}; i < sss.size(); ++i) {
    // if suffixes different ++cur_rank
    rank_tuples[i] = {sss_sorted[i], cur_rank};
  }

  // sort tuples by pos
  //[0-3, 1-2, 2-2, 3-0, 4-1, 5-0]
  std::sort(rank_tuples.begin(), rank_tuples.end(),
            [](auto left, auto right) { return left.index < right.index; });

  // overwrite
  std::vector<index_type> fps_reduced(sss.size());
  for (size_t i = 0; i < rank_tuples.size(); ++i) {
    fps_reduced[i] = rank_tuples[i].rank;
  }
  return fps_reduced;
}
}  // namespace alx::lce
/******************************************************************************/
