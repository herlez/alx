/*******************************************************************************
 * src/lce/benchmark.cpp
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

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <tlx/cmdline_parser.hpp>
#include <vector>
#include <omp.h>

#include "lce/lce_classic.hpp"
#include "lce/lce_fp.hpp"
#include "lce/lce_naive.hpp"
#include "lce/lce_naive_std.hpp"
#include "lce/lce_naive_wordwise.hpp"
#include "lce/lce_rk_prezza.hpp"
#include "lce/lce_sss.hpp"
#include "lce/lce_sss_naive.hpp"
#include "lce/lce_sss_noss.hpp"
#include "util/io.hpp"
#include "util/timer.hpp"

namespace fs = std::filesystem;

std::vector<std::string> algorithms{
    "naive",        "naive_std",   "naive_wordwise", "rk-prezza",
    "fp16",         "fp32",        "fp64",           "fp128",
    "sss_naive256", "sss_naive512", "sss_naive1024",  "sss_naive2048",
    "sss_noss256",  "sss_noss512", "sss_noss1024",   "sss_noss2048",
    "sss256",       "sss512",      "sss1024",        "sss2048",
    "classic"};
std::vector<std::string> algorithm_sets{"all", "seq", "par", "main"};

std::vector<std::string> algorithms_seq{"naive", "naive_std", "naive_wordwise"};
std::vector<std::string> algorithms_par{
    "fp16",         "fp32",        "fp64",          "fp128",
    "sss_naive256", "sss_naive512", "sss_naive1024", "sss_naive2048",
    "sss_noss256",  "sss_noss512", "sss_noss1024",  "sss_noss2048",
    "sss256",       "sss512",      "sss1024",       "sss2048",
};

std::vector<std::string> algorithms_main{
    "naive_wordwise", "fp32", "sss_naive512", "sss_noss512", "sss512"};

class benchmark {
 public:
  fs::path text_path;
  std::vector<uint8_t> text;

  fs::path queries_path;
  std::vector<size_t> queries;
  size_t num_queries = size_t{1'000'000};
  size_t lce_from = 0;
  size_t lce_to = 20;

  std::string algorithm = "naive";

  bool check_parameters() {
    // Check text path
    if (!fs::is_regular_file(text_path) || fs::file_size(text_path) == 0) {
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

    // Check algorithm
    if (std::find(algorithms.begin(), algorithms.end(), algorithm) ==
            algorithms.end() &&
        std::find(algorithm_sets.begin(), algorithm_sets.end(), algorithm) ==
            algorithm_sets.end()) {
      fmt::print("Algorithm {} is not specified.\n Use one of {} or {}\n",
                 algorithm, algorithms, algorithm_sets);
      return false;
    }
    return true;
  }

  void load_text() {
    alx::util::timer t;
    if (text.empty()) {
      text = alx::util::load_vector<uint8_t>(
          text_path, std::numeric_limits<size_t>::max(), 8);
      assert(text.size() != 0);
      assert(text.size() % 8 == 0);
    }
    fmt::print(" text={}", text_path.filename().string());
    fmt::print(" text_size={}", text.size());
    fmt::print(" text_time={}", t.get());
  }

  template <typename lce_ds_type>
  lce_ds_type benchmark_construction() {
#ifdef ALX_BENCHMARK_SPACE
    malloc_count_reset_peak();
    size_t mem_before = malloc_count_current();
#endif
    alx::util::timer t;
    lce_ds_type lce_ds(text);
    fmt::print(" threads={}", omp_get_max_threads());
    fmt::print(" c_time={}", t.get());
#ifdef ALX_BENCHMARK_SPACE
    fmt::print(" c_mem={}", malloc_count_current() - mem_before);
    fmt::print(" c_mempeak={}", malloc_count_peak() - mem_before);
#endif
    return lce_ds;
  }

  void load_queries(size_t lce_cur) {
    alx::util::timer t;
    // First load queries from file
    fs::path cur_query_path = queries_path;
    cur_query_path.append(fmt::format("lce_{}", lce_cur));
    queries = alx::util::load_vector<size_t>(cur_query_path);

    // Now clone queries until there are num_unique_queries many
    if (queries.size() != 0) {
      size_t num_unique_queries = queries.size();
      assert(num_unique_queries % 2 == 0);
      queries.resize(num_queries * 2);
      for (size_t i = num_unique_queries; i < queries.size(); ++i) {
        queries[i] = queries[i % num_unique_queries];
      }
    }
    assert(queries.size() == 0 || queries.size() == num_queries * 2);
    // fmt::print(" q_path={}", cur_query_path.string());
    fmt::print(" q_size={}", queries.size() / 2);
    fmt::print(" q_load_time={}", t.get());
  }

  template <typename lce_ds_type>
  void benchmark_queries(lce_ds_type& lce_ds) {
    if (queries.empty()) {
      return;
    }
    size_t check_sum = 0;
    alx::util::timer t;
    for (size_t i = 0; i < queries.size(); i += 2) {
      check_sum += lce_ds.lce(queries[i], queries[i + 1]);
    }

    fmt::print(" q_time={}", t.get());
    fmt::print(" check_sum={}", check_sum);
  }

  template <typename lce_ds_type>
  void run(std::string const& algo_name) {
    if (algorithm == "main") {
      if (std::find(algorithms_main.begin(), algorithms_main.end(),
                    algo_name) == algorithms_main.end()) {
        return;
      }
    } else if (algorithm == "seq") {
      if (std::find(algorithms_seq.begin(), algorithms_seq.end(), algo_name) ==
          algorithms_seq.end()) {
        return;
      }
    } else if (algorithm == "par") {
      if (std::find(algorithms_par.begin(), algorithms_par.end(), algo_name) ==
          algorithms_par.end()) {
        return;
      }
    } else if (algorithm == "all") {
      // OK
    } else {
      if (algo_name != algorithm) {
          return;
        }
    }

    // Benchmark construction
    fmt::print("RESULT algo={}", algo_name);

    load_text();

    lce_ds_type lce_ds = benchmark_construction<lce_ds_type>();
    fmt::print("\n");

    // Benchmark queries
    size_t lce_cur = lce_from;
    while (lce_cur < lce_to) {
      fmt::print("RESULT algo={}_queries", algo_name);
      fmt::print(" lce_range={}", lce_cur);
      load_queries(lce_cur);
      benchmark_queries<lce_ds_type>(lce_ds);
      fmt::print("\n");
      ++lce_cur;
    }
  }
};

int main(int argc, char** argv) {
  benchmark b;

  tlx::CmdlineParser cp;
  cp.set_description(
      "This program measures construction time and LCE query time for "
      "several "
      "LCE data structures. Generate LCE queries with gen_queries");
  cp.set_author(
      "Alexander Herlez <alexander.herlez@tu-dortmund.de>\n"
      "        Florian Kurpicz  <florian.kurpicz@tu-dortmund.de>\n"
      "        Patrick Dinklage <patrick.dinklage@tu-dortmund.de>");

  cp.add_param_path("text_path", b.text_path,
                    "The path to the text which is queried");
  cp.add_path("queries_path", b.queries_path,
              "The path to the generated queries (default="
              "text_path.remove_filename()).");
  cp.add_bytes('q', "num_queries", b.num_queries,
               "Number of LCE queries that are executed (default=1,000,000).");
  cp.add_bytes(
      "from", b.lce_from,
      "Use only lce queries which return at least 2^{from}  (default=0).");
  cp.add_bytes(
      "to", b.lce_to,
      "Use only lce queries which return up to 2^{to}-1 with (default=21)");

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

  b.run<alx::lce::lce_naive<>>("naive");
  b.run<alx::lce::lce_naive_std<>>("naive_std");
  b.run<alx::lce::lce_naive_wordwise<>>("naive_wordwise");

  b.run<alx::lce::lce_fp<uint8_t, 16>>("fp16");
  b.run<alx::lce::lce_fp<uint8_t, 32>>("fp32");
  b.run<alx::lce::lce_fp<uint8_t, 64>>("fp64");
  b.run<alx::lce::lce_fp<uint8_t, 128>>("fp128");
  b.run<rklce::lce_rk_prezza>("rk-prezza");

  b.run<alx::lce::lce_sss_naive<uint8_t, 256, uint64_t>>("sss_naive256");
  b.run<alx::lce::lce_sss_naive<uint8_t, 512, uint64_t>>("sss_naive512");
  b.run<alx::lce::lce_sss_naive<uint8_t, 1024, uint64_t>>("sss_naive1024");
  b.run<alx::lce::lce_sss_naive<uint8_t, 2048, uint64_t>>("sss_naive2048");

  b.run<alx::lce::lce_sss_noss<uint8_t, 256, uint64_t>>("sss_noss256");
  b.run<alx::lce::lce_sss_noss<uint8_t, 512, uint64_t>>("sss_noss512");
  b.run<alx::lce::lce_sss_noss<uint8_t, 1024, uint64_t>>("sss_noss1024");
  b.run<alx::lce::lce_sss_noss<uint8_t, 2048, uint64_t>>("sss_noss2048");

  b.run<alx::lce::lce_sss<uint8_t, 256, uint64_t>>("sss256");
  b.run<alx::lce::lce_sss<uint8_t, 512, uint64_t>>("sss512");
  b.run<alx::lce::lce_sss<uint8_t, 1024, uint64_t>>("sss1024");
  b.run<alx::lce::lce_sss<uint8_t, 2048, uint64_t>>("sss2048");

  // b.run<alx::lce::lce_classic<uint8_t, uint64_t>>("classic");
}