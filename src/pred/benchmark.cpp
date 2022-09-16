/*******************************************************************************
 * src/lce/benchmark.cpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

//#ifndef ALX_BENCHMARK_PARALLEL
#define ALX_MEASURE_SPACE
//#endif

#include <fmt/core.h>
#ifdef ALX_MEASURE_SPACE
#include <malloc_count/malloc_count.h>
#endif

#include <assert.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

#include "pred/binsearch_std.hpp"
#include "util/timer.hpp"

namespace fs = std::filesystem;

std::vector<std::string> algorithms{"binsearch_std"};

class benchmark {
  fs::path vec_path;
  std::vector<size_t> vec;

  size_t num_queries;
  bool no_pred = false;
  bool no_succ = false;

  std::string algorithm = "bin_search_std";

  bool check_parameters() {
    // Check text path
    /*if (!fs::is_regular_file(text_path) || fs::file_size(text_path) == 0) {
      fmt::print("Text file {} is empty or does not exist.\n",
                 text_path.string());
      return false;
    }

    // Check queries path
    if (queries_path.empty()) {
      queries_path = text_path;
      queries_path.remove_filename();
    }
    if (!fs::is_directory(queries_path)) {
      fmt::print("Query directory {} does not exist.\n", queries_path.string());
      return false;
    }
    for (size_t i = lce_from; i < lce_to; ++i) {
      fs::path q_path = queries_path;
      q_path.append(fmt::format("lce_{}", i));
      if (!fs::is_regular_file(q_path)) {
        fmt::print("Query file {} does not exist.\n", queries_path.string());
        return false;
      }
    }

    // Check algorithm flag
    if (std::find(algorithms.begin(), algorithms.end(), algorithm) ==
        algorithms.end()) {
      std::cout << fmt::format(
          "Algorithm {} is not specified.\n Use one of {}\n", algorithm,
          algorithms);
      return false;
    }*/
    return true;
  }

  void load_data() {
  }

  void load_queries() {
  }

  template <typename pred_ds_type>
  pred_ds_type benchmark_construction() {
#ifdef ALX_MEASURE_SPACE
    malloc_count_reset_peak();
    size_t mem_before = malloc_count_current();
#endif
    alx::util::timer t;
    pred_ds_type pred_ds(vec);
    fmt::print(" threads={}", omp_get_max_threads());
    fmt::print(" c_time={}", t.get());
#ifdef ALX_MEASURE_SPACE
    fmt::print(" c_mem={}", malloc_count_current() - mem_before);
    fmt::print(" c_mempeak={}", malloc_count_peak() - mem_before);
#endif
    return pred_ds;
  }

  template <typename t_data_type>
  std::vector<t_data_type> load_data() {
    // if
    return std::vector<t_data_type>();
  }

 public:
  template <typename pred_ds_type>
  void run(std::string const& algo_name) {
    if (algo_name != algorithm) {
      return;
    }

    fmt::print("RESULT algo={}", algorithm);
    load_data();
    pred_ds_type pred_ds = benchmark_construction<pred_ds>();
    load_queries();
    benchmark_queries<pred_ds_type>(pred_ds);
    fmt::print("\n");
  }

  /*void run() {
  // Load vector
  {
    if (std::filesystem::exists(vec_path)) {
      std::cout << "File " << vec_path << " does not exist.";
      return;
    };
    vec.reserve(std::filesystem::file_size(vec_path));
    std::ifstream file(vec_path, std::ios::binary);
    std::copy(std::istream_iterator<uint64_t>(file),
              std::istream_iterator<uint64_t>(), std::back_inserter(vec));
    assert(vec.size != 0);
    assert(std::is_sorted(vec));
  }

  // Generate queries
  {
    queries.resize(num_queries);
    std::mt19937 gen(59);
    std::uniform_int_distribution<uint64_t> distrib(0, vec.back());
    for (auto& i : queries) {
      i = distrib(gen);
    }
  }

  // Benchmark
  { benchmark<alx::pred::binsearch_std<uint64_t>>(vec, queries); }
}

template <typename T>
void benchmark(std::vector<uint64_t> const& vec,
               std::vector<uint64_t> const& queries) {
  // Construction
  T pred_ds(vec);

  // Predecessor queries
  if (!no_pred) {
    size_t check_sum = 0;
    for (auto q : queries) {
      check_sum += pred_ds.predecessor(q);
    }
  }

  // Successor queries
  if (!no_succ) {
    size_t check_sum = 0;
    for (auto q : queries) {
      check_sum += pred_ds.successor(q);
    }
  }
}*/
};

int main(int argc, char** argv) {
  // binary File Path
  // num_queries
  // no-pred no-succ

  std::cout << "Hello Pred/n";

  std::vector<uint32_t> vec;
  alx::pred::binsearch_std<uint32_t> pred(vec.data(), vec.size());
  alx::pred::binsearch_std<uint32_t> pred2(vec);

  predecessor_benchmark b;
  b.run();
}