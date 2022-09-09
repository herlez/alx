/*******************************************************************************
 * alx/lce/lce_fp.hpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once
#include <assert.h>

#include <bit>
#include <cstdint>

#include "rolling_hash/modular_arithmetic.hpp"
#include "rolling_hash/rolling_hash.hpp"

namespace alx::lce {

template <typename t_char_type = uint8_t>
class lce_fp {
 public:
  typedef t_char_type char_type;
  __extension__ typedef unsigned __int128 uint128_t;

  lce_fp() : m_block_fps(nullptr), m_size(0) {
  }

  lce_fp(char_type* text, size_t size)
      : m_block_fps(reinterpret_cast<uint64_t*>(text)), m_size(size) {
    // Partition text for threads in superblocks.
    size_t num_blocks = size / 64 + (size % 64 > 0);
    size_t begin = 0;
    size_t end = num_blocks;
    int t = 0;
    int nt = 1;

    // For small endian systems we need to swap the order of bytes in order to
    // calculate fingerprints. Luckily this step is fast.
    if constexpr (std::endian::native == std::endian::little) {
      for (size_t i = begin; i < end; ++i) {
        m_block_fps[i] =
            __builtin_bswap64(m_block_fps[i]);  // C++23 std::byteswap!
      }
    }

    // First calculate FP of superblock.
    std::vector<uint64_t> superblock_fps;
    superblock_fps.resize(nt);
    if (t != nt - 1) {
      uint128_t fingerprint = 0;
      for (size_t i = begin; i < end; ++i) {
        uint128_t current_block = m_block_fps[i];
        fingerprint <<= 64;
        fingerprint += current_block;
        fingerprint %= m_prime;
      }
      superblock_fps[t + 1] = fingerprint;
    }

    // Prefix sum over fingerprints of superblocks.
    // critical
    if (t == 0) {
      uint128_t shift_influence =
          modular::pow_mod(uint128_t{1} << 64, uint128_t{end - begin}, m_prime);
      for (size_t i = 1; i < superblock_fps.size(); ++i) {
        uint128_t last_block_influence =
            shift_influence * superblock_fps[i - 1] % m_prime;
        uint128_t cur_block_influence = superblock_fps[i];
        superblock_fps[i] =
            last_block_influence + cur_block_influence % m_prime;
      }
    }

    // Synchronize
    // Overwrite text with fingerprints
    {
      uint128_t fingerprint = 0;
      for (size_t i = begin; i < end; ++i) {
        uint128_t current_block = m_block_fps[i];
        fingerprint <<= 64;
        fingerprint += current_block;
        fingerprint %= m_prime;
        assert(fingerprint < 0x8000000000000000ULL);
        m_block_fps[i] = static_cast<uint64_t>(fingerprint) +
                         (current_block >= m_prime) * 0x8000000000000000ULL;
      }
    }
  }

  template <typename C>
  lce_fp(C& container) : lce_fp(container.data(), container.size()) {
  }

  char_type access(size_t pos) const {
    return 0;  // todo
  }

  // Returns the number of common letters in text[i..] and text[j..].
  size_t lce(size_t i, size_t j) const {
    if (i == j) [[unlikely]] {
      assert(i < m_size);
      return m_size - i;
    }
    return lce_uneq(i, j);
  }

  // Returns the number of common letters in text[i..] and text[j..]. Here i and
  // j must be different.
  size_t lce_uneq(size_t i, size_t j) const {
    assert(i != j);

    size_t l = std::min(i, j);
    size_t r = std::max(i, j);

    return lce_lr(l, r);
  }

  // Returns the number of common letters in text[i..] and text[j..].
  // Here l must be smaller than r.
  size_t lce_lr(size_t l, size_t r) const {
    return 0;
  }

  // Returns {b, lce}, where lce is the number of common letters in text[i..]
  // and text[j..] and b tells whether the lce ends with a mismatch.
  std::pair<bool, size_t> lce_mismatch(size_t i, size_t j) const {
    if (i == j) [[unlikely]] {
      assert(i < m_size);
      return {false, m_size - i};
    }

    size_t l = std::min(i, j);
    size_t r = std::max(i, j);

    size_t lce = lce_lr(l, r);
    return {r + lce != m_size, lce};
  }

  // Returns whether text[i..] is lexicographic smaller than text[j..]. Here i
  // and j must be different.
  bool is_leq_suffix(size_t i, size_t j) const {
    assert(i != j);
    size_t lce_val = lce_uneq(i, j);
    return (i + lce_val == m_size || access(i + lce_val) < access(j + lce_val));
  }

  // Returns {b, lce}, where lce is the lce of text[i..i+lce) and text[j..j+lce]
  // and b tells whether the lce ends with a mismatch.
  /*std::pair<bool, size_t> lce_up_to(size_t i, size_t j, size_t up_to) const {
    if (i == j) [[unlikely]] {
      assert(i < size);
      return {false, size - i};
    }

    size_t l = std::min(i, j);
    size_t r = std::max(i, j);

    size_t lce_max = std::min(r + up_to, size) - r;
    size_t lce = lce_lr(l, r);
    return {lce < lce_max, lce};
  }*/

 private:
  uint64_t* m_block_fps;
  size_t m_size;
  static constexpr uint128_t m_prime{0x800000000000001d};
};
}  // namespace alx::lce
