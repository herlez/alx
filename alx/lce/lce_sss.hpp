/*******************************************************************************
 * alx/lce/lce_sss.hpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 * Copyright (C) 2019 Florian Kurpicz <florian.kurpicz@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once

#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

//#include "util/successor/index_par.hpp"
//#include "util/util.hpp"
//#include "util_ssss_par/lce-rmq.hpp"
//#include "util_ssss_par/ssss_par.hpp"
//#include "util_ssss_par/sss_checker.hpp"

#ifdef DETAILED_TIME
#include <malloc_count.h>
#endif

namespace alx::lce {

/* This class stores a text as an array of characters and
 * answers LCE-queries with the naive method. */
template <typename t_char_type = uint8_t, uint64_t t_tau = 1024>
class lce_sss {
 public:
  typedef t_char_type char_type;
  __extension__ typedef unsigned __int128 uint128_t;

  lce_sss() : m_text(nullptr), m_size(0) {
  }

  lce_sss(char_type* text, size_t size)
      : m_text(text), m_size(size) {
        /*
#ifdef DETAILED_TIME
    size_t mem_before = malloc_count_current();
    malloc_count_reset_peak();
    std::chrono::system_clock::time_point begin =
        std::chrono::system_clock::now();
#endif

    sync_set_ = string_synchronizing_set_par<kTau, sss_type>(text_);
    // check_string_synchronizing_set(text, sync_set_);
    // print_sss();

#ifdef DETAILED_TIME
    std::chrono::system_clock::time_point end =
        std::chrono::system_clock::now();
    if (print_ss_size) {
      std::cout << "sss_construct_time="
                << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         begin)
                       .count()
                << " "
                << "sss_construct_mem=" << (malloc_count_peak() - mem_before)
                << " "
                << "sss_size=" << sync_set_.size() << " "
                << "sss_repetetive=" << std::boolalpha << sync_set_.has_runs()
                << " "
                << "sss_runs=" << sync_set_.num_runs() << " ";
    }
#endif

#ifdef DETAILED_TIME
    mem_before = malloc_count_current();
    malloc_count_reset_peak();
    begin = std::chrono::system_clock::now();
#endif

    ind_ = std::make_unique<
        stash::pred::index_par<std::vector<sss_type>, sss_type, 7>>(
        sync_set_.get_sss());

#ifdef DETAILED_TIME
    end = std::chrono::system_clock::now();
    if (print_ss_size) {
      std::cout << "pred_construct_time="
                << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         begin)
                       .count()
                << " "
                << "pred_construct_mem=" << (malloc_count_peak() - mem_before)
                << " ";
    }
#endif
    lce_rmq_ = std::make_unique<Lce_rmq_par<sss_type, kTau>>(
        text_.data(), text_length_in_bytes_, sync_set_);

    */
  }

  template <typename C>
  lce_sss(C const& container) : lce_sss(container.data(), container.size()) {
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

    size_t l = std::min(i, j);
    size_t r = std::max(i, j);

    return lce_lr(l, r);
  }

  // Return the number of common letters in text[i..] and text[j..].
  // Here l must be smaller than r.
  inline uint64_t lce_lr(size_t i, size_t j) const {
    return 0;
    /*if (TLX_UNLIKELY(i == j)) {
      return text_length_in_bytes_ - i;
    }
    if (i > j) {
      std::swap(i, j);
    }

    // naive part
    uint64_t const sync_length = 3 * kTau;
    uint64_t const max_length =
        std::min(sync_length, text_length_in_bytes_ - j);
    uint64_t lce = 0;
    for (; lce < 8; ++lce) {
      if (TLX_UNLIKELY(lce >= max_length)) {
        return max_length;
      }
      if (text_[i + lce] != text_[j + lce]) {
        return lce;
      }
    }

    lce = 0;
    uint128_t const* const text_blocks_i =
        reinterpret_cast<uint128_t const*>(text_.data() + i);
    uint128_t const* const text_blocks_j =
        reinterpret_cast<uint128_t const*>(text_.data() + j);
    for (; lce < max_length / 16; ++lce) {
      if (text_blocks_i[lce] != text_blocks_j[lce]) {
        lce *= 16;
        // The last block did not match. Here we compare its single characters
        uint64_t lce_end = std::min(lce + 16, max_length);
        for (; lce < lce_end; ++lce) {
          if (text_[i + lce] != text_[j + lce]) {
            return lce;
          }
        }
        return lce;
      }
    }
    lce *= 16;

    // strSync part
    uint64_t const i_ = suc(i + 1);
    uint64_t const j_ = suc(j + 1);

    uint64_t const i_diff = sync_set_[i_] - i;
    uint64_t const j_diff = sync_set_[j_] - j;

    if (i_diff == j_diff) {
      return i_diff + lce_rmq_->lce(i_, j_);
    } else {
      return std::min(i_diff, j_diff) + 2 * kTau - 1;
    }*/
  }
  
  

  char_type operator[](size_t i) {
    return m_text[i];
  }

  size_t size() {
    return m_size;
  }

  //size_t sss_size() {
  //  return m_sync_set.size();
  //}

  //std::vector<sss_type> getSyncSet() {
  //  return sync_set_.get_sss();
  //}

  //void print_sss() {
  //  std::ofstream of("/tmp/sss", std::ios::trunc);
  //  for (auto i : sync_set_.get_sss()) {
  //    of << i << "\n";
  //  }
  //}

 private:
  /* Finds the smallest element that is greater or equal to i
     Because s_ is ordered, that is equal to the
     first element greater than i */
  //inline sss_type suc(sss_type i) const {
  //  return m_ind->successor(i).pos;
  //}

 private:
  char_type const* m_text;
  size_t m_size;

  // stash::pred::index_par<std::vector<sss_type>, sss_type, 7> m_pred;
  // rolling_hash::sss<kTau, sss_type> m_sync_set;
  // lce_rmq_par<sss_type, t_tau> m_lce_rmq;
};
}  // namespace alx::lce
/******************************************************************************/
