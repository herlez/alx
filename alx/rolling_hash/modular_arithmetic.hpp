/*******************************************************************************
 * alx/rolling_hash/modular_arithmetic.hpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once
#include <assert.h>
#include <hurchalla/modular_arithmetic/modular_pow.h>

#include <bit>
#include <iterator>
#include <random>

namespace alx::modular {

// Return base^exp % prime.
template <typename T>
inline T pow_mod(T base, T exp, T prime) {
  return hurchalla::modular_pow(base, exp, prime);
}

}  // namespace alx::modular
