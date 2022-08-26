#include <gtest/gtest.h>

#include <limits>
#include <numeric>

#include "lce/lce_naive.hpp"
#include "lce/lce_naive_block.hpp"
#include "lce/lce_naive_std.hpp"

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

  EXPECT_EQ(ds.lce_up_to(1000, 0, 200), std::make_pair(false, size_t{200}));
  EXPECT_EQ(ds.lce_up_to(1000, 500, 200), std::make_pair(true, size_t{0}));
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
