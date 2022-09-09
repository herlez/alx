/*******************************************************************************
 * test/lce/test_lce.cpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#include <gtest/gtest.h>

#include <limits>
#include <numeric>

#include "lce/lce_naive.hpp"
#include "lce/lce_naive_block.hpp"
#include "lce/lce_naive_std.hpp"
#include "lce/lce_fp.hpp"

template <typename lce_ds_type>
void test_empty_constructor() {
  lce_ds_type ds;
}

template <typename lce_ds_type>
void test_simple() {
  typedef typename lce_ds_type::char_type char_typee;
  std::vector<char_typee> text(2000);
  std::iota(text.begin(), text.begin()+1000,
            std::numeric_limits<char_typee>::max() / 2);
  std::iota(text.begin()+1000, text.end(),
            std::numeric_limits<char_typee>::max() / 2);
  // Test pointer constructor
  {
    lce_ds_type ds(text.data(), text.size());
    EXPECT_EQ(ds.lce(0, 0), 2000);
    EXPECT_EQ(ds.lce(0, 1000), 1000);
    EXPECT_EQ(ds.lce(500, 1000), 0);
  }

  // Test container constructor
  {
    lce_ds_type ds(text);
    EXPECT_EQ(ds.lce(0, 0), 2000);
    EXPECT_EQ(ds.lce(0, 1000), 1000);
    EXPECT_EQ(ds.lce(500, 1000), 0);
  }
}

template <typename lce_ds_type>
void test_variants() {
  typedef typename lce_ds_type::char_type char_typee;
  std::vector<char_typee> text(2000);
  std::iota(text.begin(), text.begin()+1000,
            std::numeric_limits<char_typee>::max() / 2);
  std::iota(text.begin()+1000, text.end(),
            std::numeric_limits<char_typee>::max() / 2);

  lce_ds_type ds(text);
  EXPECT_EQ(ds.lce_lr(0, 1000), 1000);
  EXPECT_EQ(ds.lce(500, 1000), 0);

  EXPECT_EQ(ds.lce_mismatch(1000, 0), std::make_pair(false, size_t{1000}));
  EXPECT_EQ(ds.lce_mismatch(1000, 500), std::make_pair(true, size_t{0}));

  EXPECT_EQ(ds.is_leq_suffix(500, 1500), false);
  EXPECT_EQ(ds.is_leq_suffix(1500, 500), true);
  EXPECT_EQ(ds.is_leq_suffix(0, 500), true);
  EXPECT_EQ(ds.is_leq_suffix(500, 0), false);

  //EXPECT_EQ(ds.lce_up_to(1000, 0, 200), std::make_pair(false, size_t{200}));
  //EXPECT_EQ(ds.lce_up_to(1000, 500, 200), std::make_pair(true, size_t{0}));
}

TEST(LceNaive, All) {
  test_empty_constructor<alx::lce::lce_naive<unsigned char>>();
  test_simple<alx::lce::lce_naive<unsigned char>>();
  test_simple<alx::lce::lce_naive<char>>();
  test_simple<alx::lce::lce_naive<uint8_t>>();
  test_simple<alx::lce::lce_naive<uint32_t>>();
  test_simple<alx::lce::lce_naive<int32_t>>();
  test_simple<alx::lce::lce_naive<uint64_t>>();
  test_simple<alx::lce::lce_naive<int64_t>>();
  test_simple<alx::lce::lce_naive<__uint128_t>>();
  test_variants<alx::lce::lce_naive<__uint128_t>>();
}

TEST(LceNaiveStd, All) {
  test_empty_constructor<alx::lce::lce_naive_std<unsigned char>>();
  test_simple<alx::lce::lce_naive_std<unsigned char>>();
  test_simple<alx::lce::lce_naive_std<char>>();
  test_simple<alx::lce::lce_naive_std<uint8_t>>();
  test_simple<alx::lce::lce_naive_std<uint32_t>>();
  test_simple<alx::lce::lce_naive_std<int32_t>>();
  test_simple<alx::lce::lce_naive_std<uint64_t>>();
  test_simple<alx::lce::lce_naive_std<int64_t>>();
  test_simple<alx::lce::lce_naive_std<__uint128_t>>();
  test_variants<alx::lce::lce_naive_std<__uint128_t>>();
}

TEST(LceNaiveBlock, All) {
  test_empty_constructor<alx::lce::lce_naive_std<unsigned char>>();
  test_simple<alx::lce::lce_naive_block<unsigned char>>();
  test_simple<alx::lce::lce_naive_block<char>>();
  test_simple<alx::lce::lce_naive_block<uint8_t>>();
  test_simple<alx::lce::lce_naive_block<uint32_t>>();
  test_simple<alx::lce::lce_naive_block<int32_t>>();
  test_simple<alx::lce::lce_naive_block<uint64_t>>();
  test_simple<alx::lce::lce_naive_block<int64_t>>();
  test_simple<alx::lce::lce_naive_block<__uint128_t>>();
  test_variants<alx::lce::lce_naive_block<__uint128_t>>();
}

TEST(LceFP, All) {
  test_empty_constructor<alx::lce::lce_naive_std<unsigned char>>();
  test_simple<alx::lce::lce_fp<unsigned char>>();
  test_simple<alx::lce::lce_fp<char>>();
  test_simple<alx::lce::lce_fp<uint8_t>>();
  test_simple<alx::lce::lce_fp<uint32_t>>();
  test_simple<alx::lce::lce_fp<int32_t>>();
  test_simple<alx::lce::lce_fp<uint64_t>>();
  test_simple<alx::lce::lce_fp<int64_t>>();
  test_simple<alx::lce::lce_fp<__uint128_t>>();
  test_variants<alx::lce::lce_fp<__uint128_t>>();
}