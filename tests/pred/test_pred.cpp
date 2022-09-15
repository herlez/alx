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

#include "pred/binsearch_std.hpp"

template <typename pred_ds_type>
void test_empty_constructor() {
  pred_ds_type ds;
}

template <typename pred_ds_type>
void test_simple() {
  typedef typename pred_ds_type::data_type data_type;
  std::vector<data_type> data(100);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = i * 2 + 1;
  }

  std::vector<data_type> data_copy = data;

  // Test pointer constructor
  {
    pred_ds_type ds(data.data(), data.size());
    EXPECT_EQ(ds.template predecessor<false>(0).exists, false);
    EXPECT_EQ(ds.template predecessor<true>(0).exists, false);

    EXPECT_EQ(ds.template predecessor<false>(1),
              alx::pred::result(true, size_t{0}));
    EXPECT_EQ(ds.template predecessor<true>(1).exists, false);

    EXPECT_EQ(ds.template predecessor<false>(99),
              alx::pred::result(true, size_t{49}));
    EXPECT_EQ(ds.template predecessor<true>(99),
              alx::pred::result(true, size_t{48}));

    EXPECT_EQ(ds.template predecessor<false>(100),
              alx::pred::result(true, size_t{49}));
    EXPECT_EQ(ds.template predecessor<true>(100),
              alx::pred::result(true, size_t{49}));

    EXPECT_EQ(ds.template predecessor<false>(199),
              alx::pred::result(true, size_t{99}));
    EXPECT_EQ(ds.template predecessor<true>(199),
              alx::pred::result(true, size_t{98}));

    EXPECT_EQ(ds.template predecessor<false>(200),
              alx::pred::result(true, size_t{99}));
    EXPECT_EQ(ds.template predecessor<true>(200),
              alx::pred::result(true, size_t{99}));

    EXPECT_EQ(ds.template successor<false>(0),
              alx::pred::result(true, size_t{0}));
    EXPECT_EQ(ds.template successor<true>(0),
              alx::pred::result(true, size_t{0}));

    EXPECT_EQ(ds.template successor<false>(1),
              alx::pred::result(true, size_t{0}));
    EXPECT_EQ(ds.template successor<true>(1),
              alx::pred::result(true, size_t{1}));

    EXPECT_EQ(ds.template successor<false>(99),
              alx::pred::result(true, size_t{49}));
    EXPECT_EQ(ds.template successor<true>(99),
              alx::pred::result(true, size_t{50}));

    EXPECT_EQ(ds.template successor<false>(100),
              alx::pred::result(true, size_t{50}));
    EXPECT_EQ(ds.template successor<true>(100),
              alx::pred::result(true, size_t{50}));

    EXPECT_EQ(ds.template successor<false>(199),
              alx::pred::result(true, size_t{99}));
    EXPECT_EQ(ds.template successor<true>(199).exists, false);

    EXPECT_EQ(ds.template successor<false>(200).exists, false);
    EXPECT_EQ(ds.template successor<true>(200).exists, false);

    // Methods without bounds check
    EXPECT_EQ(ds.template predecessor_unsafe<false>(1), 0);

    EXPECT_EQ(ds.template predecessor_unsafe<false>(99), 49);
    EXPECT_EQ(ds.template predecessor_unsafe<true>(99), 48);

    EXPECT_EQ(ds.template predecessor_unsafe<false>(100), 49);
    EXPECT_EQ(ds.template predecessor_unsafe<true>(100), 49);

    EXPECT_EQ(ds.template predecessor_unsafe<false>(199), 99);
    EXPECT_EQ(ds.template predecessor_unsafe<true>(199), 98);

    EXPECT_EQ(ds.template predecessor_unsafe<false>(200), 99);
    EXPECT_EQ(ds.template predecessor_unsafe<true>(200), 99);

    EXPECT_EQ(ds.template successor_unsafe<false>(0), 0);
    EXPECT_EQ(ds.template successor_unsafe<true>(0), 0);

    EXPECT_EQ(ds.template successor_unsafe<false>(1), 0);
    EXPECT_EQ(ds.template successor_unsafe<true>(1), 1);

    EXPECT_EQ(ds.template successor_unsafe<false>(99), 49);
    EXPECT_EQ(ds.template successor_unsafe<true>(99), 50);

    EXPECT_EQ(ds.template successor_unsafe<false>(100), 50);
    EXPECT_EQ(ds.template successor_unsafe<true>(100), 50);

    EXPECT_EQ(ds.template successor_unsafe<false>(199), 99);

    // Exist method
    EXPECT_EQ(ds.contains(0), false);
    EXPECT_EQ(ds.contains(1), true);
    EXPECT_EQ(ds.contains(99), true);
    EXPECT_EQ(ds.contains(100), false);
    EXPECT_EQ(ds.contains(199), true);
    EXPECT_EQ(ds.contains(200), false);
  
  }
  data = data_copy;
}

template <typename pred_ds_type>
void test_variants() {
}

TEST(PredBinsearchStd, All) {
  test_empty_constructor<alx::pred::binsearch_std<uint64_t>>();
  test_simple<alx::pred::binsearch_std<unsigned char>>();
  test_simple<alx::pred::binsearch_std<uint8_t>>();
  test_simple<alx::pred::binsearch_std<uint32_t>>();
  test_simple<alx::pred::binsearch_std<int32_t>>();
  test_simple<alx::pred::binsearch_std<uint64_t>>();
  test_simple<alx::pred::binsearch_std<int64_t>>();
  test_simple<alx::pred::binsearch_std<__uint128_t>>();
}