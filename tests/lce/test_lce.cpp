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
#include "lce/lce_sss_naive.hpp"
#include "lce/lce_sss_noss.hpp"

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
  for (auto& c : text) {
    if (c == numeric_limits<char_typee>::max()) {
      c = 0;
    }
  }
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

template <typename lce_ds_type, bool lr = true, bool mm = true, bool suf = true,
          bool upto = true>
void test_variants() {
  typedef typename lce_ds_type::char_type char_typee;
  std::vector<char_typee> text(200);
  std::iota(text.begin(), text.begin() + 100,
            std::numeric_limits<char_typee>::max() / 2);
  std::iota(text.begin() + 100, text.end(),
            std::numeric_limits<char_typee>::max() / 2);
  for (auto& c : text) {
    if (c == numeric_limits<char_typee>::max()) {
      c = 0;
    }
  }
  lce_ds_type ds(text);
  if constexpr (lr) {
    EXPECT_EQ(ds.lce_lr(0, 100), 100);
    EXPECT_EQ(ds.lce(50, 100), 0);
  }

  if constexpr (mm) {
    EXPECT_EQ(ds.lce_mismatch(100, 0), std::make_pair(false, size_t{100}));
    EXPECT_EQ(ds.lce_mismatch(100, 50), std::make_pair(true, size_t{0}));
  }
  if constexpr (suf) {
    EXPECT_EQ(ds.is_leq_suffix(50, 150), false);
    EXPECT_EQ(ds.is_leq_suffix(150, 50), true);
    EXPECT_EQ(ds.is_leq_suffix(0, 50), true);
    EXPECT_EQ(ds.is_leq_suffix(50, 0), false);
  }
  if constexpr (upto) {
    EXPECT_EQ(ds.lce_up_to(100, 0, 20), size_t{20});
    EXPECT_EQ(ds.lce_up_to(100, 50, 20), size_t{0});
  }
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
  test_empty_constructor<alx::lce::lce_naive<uint8_t>>();

  test_simple<alx::lce::lce_naive<uint8_t>>();
  test_simple<alx::lce::lce_naive<int8_t>>();
  test_simple<alx::lce::lce_naive<uint16_t>>();
  test_simple<alx::lce::lce_naive<int16_t>>();
  test_simple<alx::lce::lce_naive<uint32_t>>();
  test_simple<alx::lce::lce_naive<int32_t>>();
  test_simple<alx::lce::lce_naive<uint64_t>>();
  test_simple<alx::lce::lce_naive<int64_t>>();
  test_simple<alx::lce::lce_naive<__uint128_t>>();
  test_simple<alx::lce::lce_naive<__int128_t>>();

  test_variants<alx::lce::lce_naive<uint8_t>>();
  test_variants<alx::lce::lce_naive<int8_t>>();
  test_variants<alx::lce::lce_naive<uint16_t>>();
  test_variants<alx::lce::lce_naive<int16_t>>();
  test_variants<alx::lce::lce_naive<uint32_t>>();
  test_variants<alx::lce::lce_naive<int32_t>>();
  test_variants<alx::lce::lce_naive<uint64_t>>();
  test_variants<alx::lce::lce_naive<int64_t>>();
  test_variants<alx::lce::lce_naive<__uint128_t>>();
  test_variants<alx::lce::lce_naive<__int128_t>>();
}

TEST(LceNaiveStd, All) {
  test_empty_constructor<alx::lce::lce_naive_std<uint8_t>>();

  test_simple<alx::lce::lce_naive_std<uint8_t>>();
  test_simple<alx::lce::lce_naive_std<int8_t>>();
  test_simple<alx::lce::lce_naive_std<uint16_t>>();
  test_simple<alx::lce::lce_naive_std<int16_t>>();
  test_simple<alx::lce::lce_naive_std<uint32_t>>();
  test_simple<alx::lce::lce_naive_std<int32_t>>();
  test_simple<alx::lce::lce_naive_std<uint64_t>>();
  test_simple<alx::lce::lce_naive_std<int64_t>>();
  test_simple<alx::lce::lce_naive_std<__uint128_t>>();
  test_simple<alx::lce::lce_naive_std<__int128_t>>();

  test_variants<alx::lce::lce_naive_std<uint8_t>>();
  test_variants<alx::lce::lce_naive_std<int8_t>>();
  test_variants<alx::lce::lce_naive_std<uint16_t>>();
  test_variants<alx::lce::lce_naive_std<int16_t>>();
  test_variants<alx::lce::lce_naive_std<uint32_t>>();
  test_variants<alx::lce::lce_naive_std<int32_t>>();
  test_variants<alx::lce::lce_naive_std<uint64_t>>();
  test_variants<alx::lce::lce_naive_std<int64_t>>();
  test_variants<alx::lce::lce_naive_std<__uint128_t>>();
  test_variants<alx::lce::lce_naive_std<__int128_t>>();
}

TEST(LceNaiveWordwise, All) {
  test_empty_constructor<alx::lce::lce_naive_wordwise<uint8_t>>();

  test_simple<alx::lce::lce_naive_wordwise<uint8_t>>();
  test_simple<alx::lce::lce_naive_wordwise<int8_t>>();
  test_simple<alx::lce::lce_naive_wordwise<uint16_t>>();
  test_simple<alx::lce::lce_naive_wordwise<int16_t>>();
  test_simple<alx::lce::lce_naive_wordwise<uint32_t>>();
  test_simple<alx::lce::lce_naive_wordwise<int32_t>>();
  test_simple<alx::lce::lce_naive_wordwise<uint64_t>>();
  test_simple<alx::lce::lce_naive_wordwise<int64_t>>();
  test_simple<alx::lce::lce_naive_wordwise<__uint128_t>>();
  test_simple<alx::lce::lce_naive_wordwise<__int128_t>>();

  test_variants<alx::lce::lce_naive_wordwise<uint8_t>>();
  test_variants<alx::lce::lce_naive_wordwise<int8_t>>();
  test_variants<alx::lce::lce_naive_wordwise<uint16_t>>();
  test_variants<alx::lce::lce_naive_wordwise<int16_t>>();
  test_variants<alx::lce::lce_naive_wordwise<uint32_t>>();
  test_variants<alx::lce::lce_naive_wordwise<int32_t>>();
  test_variants<alx::lce::lce_naive_wordwise<uint64_t>>();
  test_variants<alx::lce::lce_naive_wordwise<int64_t>>();
  test_variants<alx::lce::lce_naive_wordwise<__uint128_t>>();
  test_variants<alx::lce::lce_naive_wordwise<__int128_t>>();
}

TEST(LceClassic, All) {
  test_empty_constructor<alx::lce::lce_classic<unsigned char>>();

  test_simple<alx::lce::lce_classic<uint8_t>>();
  // test_simple<alx::lce::lce_classic<int8_t>>();
  test_simple<alx::lce::lce_classic<uint16_t>>();
  // test_simple<alx::lce::lce_classic<int16_t>>();
  test_simple<alx::lce::lce_classic<uint32_t>>();
  // test_simple<alx::lce::lce_classic<int32_t>>();
  test_simple<alx::lce::lce_classic<uint64_t>>();
  // test_simple<alx::lce::lce_classic<int64_t>>();
  test_simple<alx::lce::lce_classic<__uint128_t>>();
  // test_simple<alx::lce::lce_classic<__int128_t>>();

  test_variants<alx::lce::lce_classic<uint8_t>, true, true, true, false>();
  // test_variants<alx::lce::lce_classic<int8_t>, true, true, true, false>();
  test_variants<alx::lce::lce_classic<uint16_t>, true, true, true, false>();
  // test_variants<alx::lce::lce_classic<int16_t>, true, true, true, false>();
  test_variants<alx::lce::lce_classic<uint32_t>, true, true, true, false>();
  // test_variants<alx::lce::lce_classic<int32_t>, true, true, true, false>();
  test_variants<alx::lce::lce_classic<uint64_t>, true, true, true, false>();
  // test_variants<alx::lce::lce_classic<int64_t>, true, true, true, false>();
  test_variants<alx::lce::lce_classic<__uint128_t>, true, true, true, false>();
  // test_variants<alx::lce::lce_classic<__int128_t>, true, true, true,
  // false>();
}

TEST(LceSssNaive, All) {
  test_empty_constructor<alx::lce::lce_sss_naive<uint8_t, 16>>();

  test_simple<alx::lce::lce_sss_naive<uint8_t, 16>>();
  test_simple<alx::lce::lce_sss_naive<int8_t, 16>>();
  // test_simple<alx::lce::lce_sss_naive<uint16_t, 16>>();
  // test_simple<alx::lce::lce_sss_naive<int16_t, 16>>();
  // test_simple<alx::lce::lce_sss_naive<uint32_t, 16>>();
  // test_simple<alx::lce::lce_sss_naive<int32_t, 16>>();
  // test_simple<alx::lce::lce_sss_naive<uint64_t, 16>>();
  // test_simple<alx::lce::lce_sss_naive<int64_t, 16>>();
  // test_simple<alx::lce::lce_sss_naive<__uint128_t, 16>>();
  // test_simple<alx::lce::lce_sss_naive<__int128_t, 16>>();

  test_variants<alx::lce::lce_sss_naive<int8_t, 16>>();
  test_variants<alx::lce::lce_sss_naive<uint8_t, 16>>();
  // test_variants<alx::lce::lce_sss_naive<uint16_t, 16>>();
  // test_variants<alx::lce::lce_sss_naive<int16_t, 16>>();
  // test_variants<alx::lce::lce_sss_naive<uint32_t, 16>>();
  // test_variants<alx::lce::lce_sss_naive<int32_t, 16>>();
  // test_variants<alx::lce::lce_sss_naive<uint64_t, 16>>();
  // test_variants<alx::lce::lce_sss_naive<int64_t, 16>>();
  // test_variants<alx::lce::lce_sss_naive<__uint128_t, 16>>();
  // test_variants<alx::lce::lce_sss_naive<__int128_t, 16>>();
}

TEST(LceSssNoSs, All) {
  test_empty_constructor<alx::lce::lce_sss_noss<uint8_t, 16>>();

  test_simple<alx::lce::lce_sss_noss<uint8_t, 16>>();
  test_simple<alx::lce::lce_sss_noss<int8_t, 16>>();
  // test_simple<alx::lce::lce_sss_noss<uint16_t, 16>>();
  // test_simple<alx::lce::lce_sss_noss<int16_t, 16>>();
  // test_simple<alx::lce::lce_sss_noss<uint32_t, 16>>();
  // test_simple<alx::lce::lce_sss_noss<int32_t, 16>>();
  // test_simple<alx::lce::lce_sss_noss<uint64_t, 16>>();
  // test_simple<alx::lce::lce_sss_noss<int64_t, 16>>();
  // test_simple<alx::lce::lce_sss_noss<__uint128_t, 16>>();
  // test_simple<alx::lce::lce_sss_noss<__int128_t, 16>>();

  test_variants<alx::lce::lce_sss_noss<uint8_t, 16>, true, true, true, false>();
  test_variants<alx::lce::lce_sss_noss<int8_t, 16>, true, true, true, false>();
  // test_variants<alx::lce::lce_sss_noss<uint16_t, 16>>();
  // test_variants<alx::lce::lce_sss_noss<int16_t, 16>>();
  // test_variants<alx::lce::lce_sss_noss<uint32_t, 16>>();
  // test_variants<alx::lce::lce_sss_noss<int32_t, 16>>();
  // test_variants<alx::lce::lce_sss_noss<uint64_t, 16>>();
  // test_variants<alx::lce::lce_sss_noss<int64_t, 16>>();
  // test_variants<alx::lce::lce_sss_noss<__uint128_t, 16>>();
  // test_variants<alx::lce::lce_sss_noss<__int128_t, 16>>();
}

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

  test_simple<alx::lce::lce_fp<uint8_t>>();
  test_simple<alx::lce::lce_fp<int8_t>>();
  // test_simple<alx::lce::lce_fp<uint16_t>>();
  // test_simple<alx::lce::lce_fp<int16_t>>();
  // test_simple<alx::lce::lce_fp<uint32_t>>();
  // test_simple<alx::lce::lce_fp<int32_t>>();
  // test_simple<alx::lce::lce_fp<uint64_t>>();
  // test_simple<alx::lce::lce_fp<int64_t>>();
  // test_simple<alx::lce::lce_fp<__uint128_t>>();
  // test_simple<alx::lce::lce_fp<__int128_t>>();

  test_variants<alx::lce::lce_fp<uint8_t>>();
  test_variants<alx::lce::lce_fp<int8_t>>();
  // test_variants<alx::lce::lce_fp<uint16_t>>();
  // test_variants<alx::lce::lce_fp<int16_t>>();
  // test_variants<alx::lce::lce_fp<uint32_t>>();
  // test_variants<alx::lce::lce_fp<int32_t>>();
  // test_variants<alx::lce::lce_fp<uint64_t>>();
  // test_variants<alx::lce::lce_fp<int64_t>>();
  // test_variants<alx::lce::lce_fp<__uint128_t>>();
  // test_variants<alx::lce::lce_fp<__int128_t>>();
}

TEST(LceRkPrezza, All) {
  test_empty_constructor<rklce::lce_rk_prezza>();
  // test_retransform<rklce::lce_rk_prezza>();

  test_simple<rklce::lce_rk_prezza>();
  // test_variants<rklce::lce_rk_prezza>();
}