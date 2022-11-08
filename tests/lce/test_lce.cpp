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

#include "lce/lce_classic.hpp"
#include "lce/lce_fp.hpp"
#include "lce/lce_memcmp.hpp"
#include "lce/lce_naive.hpp"
#include "lce/lce_naive_std.hpp"
#include "lce/lce_naive_wordwise.hpp"
#include "lce/lce_rk_prezza.hpp"
#include "lce/lce_sss.hpp"
#include "lce/lce_sss_naive.hpp"

template <typename lce_ds_type>
void test_empty_constructor() {
  lce_ds_type ds;
}

template <typename lce_ds_type>
void test_simple() {
  typedef typename lce_ds_type::char_type char_typee;
  std::vector<char_typee> text(2000);
  std::iota(text.begin(), text.begin() + 1000,
            std::numeric_limits<char_typee>::max() / 2);
  std::iota(text.begin() + 1000, text.end(),
            std::numeric_limits<char_typee>::max() / 2);
  std::vector<char_typee> text_copy = text;
  // Test pointer constructor
  {
    lce_ds_type ds(text.data(), text.size());
    EXPECT_EQ(ds.lce(0, 0), 2000);
    EXPECT_EQ(ds.lce(0, 1000), 1000);
    EXPECT_EQ(ds.lce(500, 1000), 0);
  }
  text = text_copy;

  // Test container constructor
  {
    lce_ds_type ds(text);
    EXPECT_EQ(ds.lce(0, 0), 2000);
    EXPECT_EQ(ds.lce(0, 1000), 1000);
    EXPECT_EQ(ds.lce(500, 1000), 0);
  }
  text = text_copy;
}

template <typename lce_ds_type>
void test_simple_terminated(bool start_terminator = false) {
  typedef typename lce_ds_type::char_type char_typee;
  std::vector<char_typee> text(2000);
  std::iota(text.begin(), text.begin() + 1000,
            std::numeric_limits<char_typee>::max() / 2);
  std::iota(text.begin() + 1000, text.end(),
            std::numeric_limits<char_typee>::max() / 2);

  for (auto& c : text) {
    if (c == 0) {
      c++;
    }
  }
  if (start_terminator) {
    text.front() = 0;
  }
  text.push_back(0);

  std::vector<char_typee> text_copy = text;
  // Test pointer constructor
  {
    lce_ds_type ds(text.data(), text.size());
    EXPECT_EQ(ds.lce(0, 0), 2001);
    EXPECT_EQ(ds.lce(1, 1001), 999);
    EXPECT_EQ(ds.lce(500, 1000), 0);
  }
  text = text_copy;

  // Test container constructor
  {
    lce_ds_type ds(text);
    EXPECT_EQ(ds.lce(0, 0), 2001);
    EXPECT_EQ(ds.lce(1, 1001), 999);
    EXPECT_EQ(ds.lce(500, 1000), 0);
  }
  text = text_copy;
}

template <typename lce_ds_type>
void test_suffix_sorting() {
  typedef typename lce_ds_type::char_type char_typee;
  std::vector<char_typee> text(200);
  std::iota(text.begin(), text.begin() + 100,
            std::numeric_limits<char_typee>::max() / 2);
  std::iota(text.begin() + 100, text.end(),
            std::numeric_limits<char_typee>::max() / 2);

  lce_ds_type ds(text);
  EXPECT_EQ(ds.is_leq_suffix(50, 150), false);
  EXPECT_EQ(ds.is_leq_suffix(150, 50), true);
  EXPECT_EQ(ds.is_leq_suffix(0, 50), true);
  EXPECT_EQ(ds.is_leq_suffix(50, 0), false);
}

template <typename lce_ds_type>
void test_variants() {
  typedef typename lce_ds_type::char_type char_typee;
  std::vector<char_typee> text(200);
  std::iota(text.begin(), text.begin() + 100,
            std::numeric_limits<char_typee>::max() / 2);
  std::iota(text.begin() + 100, text.end(),
            std::numeric_limits<char_typee>::max() / 2);

  lce_ds_type ds(text);
  EXPECT_EQ(ds.lce_lr(0, 100), 100);
  EXPECT_EQ(ds.lce(50, 100), 0);

  EXPECT_EQ(ds.lce_mismatch(100, 0), std::make_pair(false, size_t{100}));
  EXPECT_EQ(ds.lce_mismatch(100, 50), std::make_pair(true, size_t{0}));

  EXPECT_EQ(ds.is_leq_suffix(50, 150), false);
  EXPECT_EQ(ds.is_leq_suffix(150, 50), true);
  EXPECT_EQ(ds.is_leq_suffix(0, 50), true);
  EXPECT_EQ(ds.is_leq_suffix(50, 0), false);

  EXPECT_EQ(ds.lce_up_to(100, 0, 20), size_t{20});
  EXPECT_EQ(ds.lce_up_to(100, 50, 20), size_t{0});
}

template <typename lce_ds_type>
void test_variants_terminated(bool start_terminator = false) {
  typedef typename lce_ds_type::char_type char_typee;
  std::vector<char_typee> text(200);
  std::iota(text.begin(), text.begin() + 100,
            std::numeric_limits<char_typee>::max() / 2);
  std::iota(text.begin() + 100, text.end(),
            std::numeric_limits<char_typee>::max() / 2);

  for (auto& c : text) {
    if (c == 0) {
      c++;
    }
  }
  if (start_terminator) {
    text.front() = 0;
  }
  text.push_back(0);

  lce_ds_type ds(text);
  EXPECT_EQ(ds.lce_lr(1, 101), 99);
  EXPECT_EQ(ds.lce(50, 100), 0);

  if (start_terminator) {
    EXPECT_EQ(ds.lce_mismatch(200, 0), std::make_pair(false, size_t{1}));
  } else {
    EXPECT_EQ(ds.lce_mismatch(200, 0), std::make_pair(true, size_t{0}));
  }

  EXPECT_EQ(ds.lce_mismatch(100, 50), std::make_pair(true, size_t{0}));

  EXPECT_EQ(ds.is_leq_suffix(50, 150), false);
  EXPECT_EQ(ds.is_leq_suffix(150, 50), true);
  EXPECT_EQ(ds.is_leq_suffix(1, 51), true);
  EXPECT_EQ(ds.is_leq_suffix(51, 1), false);

  EXPECT_EQ(ds.lce_up_to(101, 1, 20), size_t{20});
  EXPECT_EQ(ds.lce_up_to(101, 51, 20), size_t{0});
}

template <typename lce_ds_type>
void test_retransform() {
  typedef typename lce_ds_type::char_type char_typee;
  std::vector<char_typee> text(2000);
  std::iota(text.begin(), text.begin() + 1000,
            std::numeric_limits<char_typee>::max() / 2);
  std::iota(text.begin() + 1000, text.end(),
            std::numeric_limits<char_typee>::max() / 2);

  std::vector<char_typee> text_copy = text;
  lce_ds_type ds(text);

  for (size_t i = 0; i < text.size(); ++i) {
    ASSERT_EQ(ds[i], text_copy[i]) << i;
  }
  ds.retransform_text();
  for (size_t i = 0; i < text.size(); ++i) {
    ASSERT_EQ(text[i], text_copy[i]) << i;
  }
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

  test_variants<alx::lce::lce_naive<unsigned char>>();
  test_variants<alx::lce::lce_naive<char>>();
  test_variants<alx::lce::lce_naive<uint8_t>>();
  test_variants<alx::lce::lce_naive<uint32_t>>();
  test_variants<alx::lce::lce_naive<int32_t>>();
  test_variants<alx::lce::lce_naive<uint64_t>>();
  test_variants<alx::lce::lce_naive<int64_t>>();
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

  test_variants<alx::lce::lce_naive_std<unsigned char>>();
  test_variants<alx::lce::lce_naive_std<char>>();
  test_variants<alx::lce::lce_naive_std<uint8_t>>();
  test_variants<alx::lce::lce_naive_std<uint32_t>>();
  test_variants<alx::lce::lce_naive_std<int32_t>>();
  test_variants<alx::lce::lce_naive_std<uint64_t>>();
  test_variants<alx::lce::lce_naive_std<int64_t>>();
  test_variants<alx::lce::lce_naive_std<__uint128_t>>();
}

TEST(LceNaiveWordwise, All) {
  test_empty_constructor<alx::lce::lce_naive_wordwise<unsigned char>>();
  test_simple<alx::lce::lce_naive_wordwise<unsigned char>>();
  test_simple<alx::lce::lce_naive_wordwise<char>>();
  test_simple<alx::lce::lce_naive_wordwise<uint8_t>>();
  test_simple<alx::lce::lce_naive_wordwise<uint32_t>>();
  test_simple<alx::lce::lce_naive_wordwise<int32_t>>();
  test_simple<alx::lce::lce_naive_wordwise<uint64_t>>();
  test_simple<alx::lce::lce_naive_wordwise<int64_t>>();
  test_simple<alx::lce::lce_naive_wordwise<__uint128_t>>();

  test_variants<alx::lce::lce_naive_wordwise<unsigned char>>();
  test_variants<alx::lce::lce_naive_wordwise<char>>();
  test_variants<alx::lce::lce_naive_wordwise<uint8_t>>();
  test_variants<alx::lce::lce_naive_wordwise<uint32_t>>();
  test_variants<alx::lce::lce_naive_wordwise<int32_t>>();
  test_variants<alx::lce::lce_naive_wordwise<uint64_t>>();
  test_variants<alx::lce::lce_naive_wordwise<int64_t>>();
  test_variants<alx::lce::lce_naive_wordwise<__uint128_t>>();
}

TEST(LceClassic, All) {
  test_empty_constructor<alx::lce::lce_classic<unsigned char>>();

  test_simple_terminated<alx::lce::lce_classic<uint8_t>>(false);
  test_simple_terminated<alx::lce::lce_classic<uint32_t>>(true);
  test_simple_terminated<alx::lce::lce_classic<uint64_t>>(true);
  test_simple_terminated<alx::lce::lce_classic<__uint128_t>>(true);

  test_variants_terminated<alx::lce::lce_classic<uint8_t>>(false);
  test_variants_terminated<alx::lce::lce_classic<uint32_t>>(true);
  test_variants_terminated<alx::lce::lce_classic<uint64_t>>(true);
  test_variants_terminated<alx::lce::lce_classic<__uint128_t>>(true);
}

TEST(LceSssNaive, All) {
  test_empty_constructor<alx::lce::lce_sss_naive<unsigned char, 16>>();
  test_simple<alx::lce::lce_sss_naive<unsigned char, 16>>();
  test_simple<alx::lce::lce_sss_naive<char, 16>>();
  test_simple<alx::lce::lce_sss_naive<uint8_t, 16>>();

  test_variants<alx::lce::lce_sss_naive<unsigned char, 16>>();
  test_variants<alx::lce::lce_sss_naive<char, 16>>();
  test_variants<alx::lce::lce_sss_naive<uint8_t, 16>>();
}

/*TEST(LceSssNoSort, All) {
  test_empty_constructor<alx::lce::lce_sss_no_sort<unsigned char>>();
  test_simple<alx::lce::lce_sss_no_sort<unsigned char>>();
  test_simple<alx::lce::lce_sss_no_sort<char>>();
  test_simple<alx::lce::lce_sss_no_sort<uint8_t>>();

  test_variants<alx::lce::lce_sss_no_sort<unsigned char>>();
  test_variants<alx::lce::lce_sss_no_sort<char>>();
  test_variants<alx::lce::lce_sss_no_sort<uint8_t>>();
  }*/

/*TEST(LceSss, All) {
  test_empty_constructor<alx::lce::lce_sss<unsigned char, 16>>();
  test_simple<alx::lce::lce_sss<unsigned char, 16>>();
  test_simple<alx::lce::lce_sss<char, 16>>();
  test_simple<alx::lce::lce_sss<uint8_t, 16>>();

  test_variants<alx::lce::lce_sss<unsigned char, 16>>();
  test_variants<alx::lce::lce_sss<char, 16>>();
  test_variants<alx::lce::lce_sss<uint8_t, 16>>();
}*/

TEST(LceMemcmp, SS) {
  test_empty_constructor<alx::lce::lce_memcmp>();
  test_suffix_sorting<alx::lce::lce_memcmp>();
}

TEST(LceFP, All) {
  test_empty_constructor<alx::lce::lce_naive_std<unsigned char>>();
  test_retransform<alx::lce::lce_fp<unsigned char>>();
  test_simple<alx::lce::lce_fp<unsigned char>>();
  test_simple<alx::lce::lce_fp<char>>();
  test_simple<alx::lce::lce_fp<uint8_t>>();

  test_variants<alx::lce::lce_fp<unsigned char>>();
  test_variants<alx::lce::lce_fp<char>>();
  test_variants<alx::lce::lce_fp<uint8_t>>();
}

TEST(LceRkPrezza, All) {
  test_empty_constructor<rklce::lce_rk_prezza>();
  test_simple<rklce::lce_rk_prezza>();
}