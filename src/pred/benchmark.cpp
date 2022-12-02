/*******************************************************************************
 * src/pred/benchmark.cpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#include <fmt/core.h>
#include <fmt/ranges.h>
#ifdef ALX_BENCHMARK_SPACE
#include <malloc_count/malloc_count.h>
#endif

#include <assert.h>
#include <omp.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <tlx/cmdline_parser.hpp>
#include <vector>

#include "pred/binsearch_std.hpp"
#include "pred/j_index.hpp"
#include "pred/pgm_index.hpp"
#include "pred/pred_index.hpp"
#include "util/io.hpp"
#include "util/timer.hpp"

namespace fs = std::filesystem;

std::vector<std::string> algorithms{"all", "binsearch_std", "index", "j_index",
                                    "pgm"};

class benchmark {
 public:
  typedef uint64_t t_data_type;

  fs::path data_path;
  std::vector<t_data_type> data;

  std::vector<size_t> queries;
  size_t num_queries = 1'000'000;
  bool no_pred = false;
  bool no_succ = false;

  std::string algorithm = "binsearch_std";

  bool check_parameters() {
    // Check data path
    if (!fs::is_regular_file(data_path) || fs::file_size(data_path) == 0) {
      fmt::print("Text file {} is empty or does not exist.\n",
                 data_path.string());
      return false;
    }
    // Check algorithm
    if (std::find(algorithms.begin(), algorithms.end(), algorithm) ==
        algorithms.end()) {
      fmt::print("Algorithm {} is not specified.\n Use one of {}\n", algorithm,
                 algorithms);
      return false;
    }
    return true;
  }

  void load_data() {
    alx::util::timer t;
    data = alx::util::load_vector<t_data_type>(data_path);
    assert(data.size() != 0);
    fmt::print(" data={}", data_path.filename().string());
    fmt::print(" data_size={}", data.size());
    fmt::print(" data_time={}", t.get());
  }

  void load_queries() {
    alx::util::timer t;
    queries.resize(num_queries);
    // std::random_device rd;
    std::mt19937 gen(1337);
    std::uniform_int_distribution<> distrib(0, data.back());
    for (size_t i = 0; i < num_queries; ++i) {
      queries[i] = distrib(gen);
    }
    fmt::print(" q_size={}", queries.size());
    fmt::print(" q_gen_time={}", t.get());
  }

  template <typename pred_ds_type>
  pred_ds_type benchmark_construction() {
#ifdef ALX_BENCHMARK_SPACE
    malloc_count_reset_peak();
    size_t mem_before = malloc_count_current();
#endif
    alx::util::timer t;
    pred_ds_type pred_ds(data);
    fmt::print(" threads={}", omp_get_max_threads());
    fmt::print(" c_time={}", t.get());
#ifdef ALX_BENCHMARK_SPACE
    fmt::print(" c_mem={}", malloc_count_current() - mem_before);
    fmt::print(" c_mempeak={}", malloc_count_peak() - mem_before);
#endif
    return pred_ds;
  }

  template <typename pred_ds_type>
  void benchmark_queries(pred_ds_type& pred_ds) {
    if (!no_pred) {
      alx::util::timer t;
      size_t check_sum = 0;
      for (size_t i = 0; i < queries.size(); ++i) {
        check_sum += pred_ds.predecessor(queries[i]).pos;
      }
      fmt::print(" pred_time={}", t.get());
      fmt::print(" check_sum={}", check_sum);
    }

    if (!no_succ) {
      alx::util::timer t;
      size_t check_sum = 0;
      for (size_t i = 0; i < queries.size(); ++i) {
        check_sum += pred_ds.successor(queries[i]).pos;
      }
      fmt::print(" succ_time={}", t.get());
      fmt::print(" check_sum={}", check_sum);
    }
  }

 public:
  template <typename pred_ds_type>
  void run(std::string const& algo_name) {
    if (algo_name != algorithm && algorithm != "all") {
      return;
    }

    // Construction
    fmt::print("RESULT algo={}", algo_name);
    load_data();
    pred_ds_type pred_ds = benchmark_construction<pred_ds_type>();

    // Queries
    load_queries();
    benchmark_queries<pred_ds_type>(pred_ds);
    fmt::print("\n");
  }
};

int main(int argc, char** argv) {
  benchmark b;
  tlx::CmdlineParser cp;
  cp.set_description(
      "This program measures construction time and query time for several "
      "predecessor data structures.");
  cp.set_author("Alexander Herlez <alexander.herlez@tu-dortmund.de>");
  cp.add_param_path("data_path", b.data_path,
                    "The path to the integers queried");
  cp.add_bytes('q', "num_queries", b.num_queries,
               "Number of queries that are executed (default=1,000,000).");
  cp.add_flag("no_pred", b.no_pred, "Don't benchmark predecessor queries.");
  cp.add_flag("no_succ", b.no_succ, "Don't benchmark successor queries.");

  cp.add_string(
      'a', "algorithm", b.algorithm,
      fmt::format("Name of data structure which is benchmarked. Options: {}",
                  algorithms));

  if (!cp.process(argc, argv)) {
    std::exit(EXIT_FAILURE);
  }
  if (!b.check_parameters()) {
    return -1;
  }

  b.run<alx::pred::binsearch_std<uint64_t>>("binsearch_std");
  b.run<alx::pred::j_index<uint64_t>>("j_index");
  b.run<alx::pred::pred_index<uint64_t, 7, uint32_t>>("pred_index");
  b.run<alx::pred::pgm_index<uint64_t, 32>>("pgm_index");
}