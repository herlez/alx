#include <fmt/core.h>
#include <fmt/ranges.h>

#include <filesystem>
#include <iostream>
#include <tlx/cmdline_parser.hpp>
#include <vector>

#include "lce/lce_naive.hpp"
#include "util/io.hpp"
#include "util/timer.hpp"

//#include "lce/lce_naive_block.hpp"
//#include "lce/lce_naive_std.hpp"

namespace fs = std::filesystem;

std::vector<std::string> algorithms{"naive"};

class benchmark {
 public:
  fs::path text_path;
  std::vector<uint8_t> text;

  fs::path queries_path;
  std::vector<size_t> queries;
  size_t num_lce_queries = size_t{1'000'000};
  size_t lce_from = 0;
  size_t lce_to = 20;

  std::string algorithm = "naive";

  bool check_parameters() {
    // Check text path
    if (!fs::exists(text_path) || fs::file_size(text_path) == 0) {
      fmt::print("Text file {} is empty or does not exist.\n",
                 text_path.string());
      return false;
    }

    // Check queries path
    if (queries_path.empty()) {
      queries_path = text_path;
      queries_path.remove_filename();
    }
    if (!fs::exists(queries_path)) {
      fmt::print("Query directory {} does not exist.\n", queries_path.string());
      return false;
    }
    for (size_t i = lce_from; i < lce_to; ++i) {
      fs::path q_path = queries_path;
      q_path.append(fmt::format("lce_{}", i));
      if (!fs::exists(q_path)) {
        fmt::print("Query file {} does not exist.\n", queries_path.string());
        return false;
      }
    }

    // Check algorithm flag
    if (std::find(algorithms.begin(), algorithms.end(), algorithm) ==
        algorithms.end()) {
      std::cout << fmt::format("Algorithm {} is not specified.\n Use one of {}",
                               algorithm, algorithms);
      return false;
    }
    return true;
  }

  void load_text() {
    alx::util::timer t;
    text = alx::util::load_text(text_path);
    assert(text.size() != 0);
    fmt::print(" text={}", text_path.filename().string());
    fmt::print(" text_size={}", text.size());
    fmt::print(" text_time={}", t.get());
  }

  template <typename lce_ds_type>
  lce_ds_type benchmark_construction() {
    alx::util::timer t;
    lce_ds_type lce_ds(text);
    fmt::print(" c_time={}", t.get());

    return lce_ds;
  }

  void load_queries() {
    alx::util::timer t;
    // First load queries from file
    queries.clear();
    fs::path cur_query_path = queries_path;
    cur_query_path.append(fmt::format("lce_{}", lce_from));
    std::ifstream stream(cur_query_path);
    std::string line;
    while (std::getline(stream, line) && queries.size() < num_lce_queries * 2) {
      queries.push_back(std::stoll(line));
    }

    // Now clone queries until there are num_unique_queries many
    if (queries.size() != 0) {
      size_t num_unique_queries = queries.size();
      assert(num_unique_queries % 2 == 0);
      queries.resize(num_lce_queries * 2);
      for (size_t i = num_unique_queries; i < queries.size(); ++i) {
        queries[i] = queries[i % num_unique_queries];
      }
    }
    assert(queries.size() == 0 || queries.size() == num_lce_queries * 2);
    fmt::print(" q_path={}", cur_query_path.string());
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
    if (algorithm == "naive") {
      for (size_t i = 0; i < queries.size(); i += 2) {
        check_sum += lce_ds.lce(queries[i], queries[i + 1]);
      }
    } //else if (algorithm == ) {}
    fmt::print(" q_time={}", t.get());
    fmt::print(" check_sum={}", check_sum);
  }

  template <typename lce_ds_type>
  void run() {
    lce_ds_type lce_ds;
    if (!check_parameters()) {
      return;
    }
    fmt::print("RESULT algo={}", algorithm);
    load_text();
    lce_ds = benchmark_construction<lce_ds_type>();
    fmt::print("\n");
    while (lce_from < lce_to) {
      fmt::print("RESULT algo={}_queries", algorithm);
      fmt::print(" lce_range={}", lce_from);
      load_queries();
      benchmark_queries<lce_ds_type>(lce_ds);
      fmt::print("\n");
      ++lce_from;
    }
  }
};

int main(int argc, char** argv) {
  benchmark b;

  tlx::CmdlineParser cp;
  cp.set_description(
      "This program measures construction time and LCE query time for several "
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
  cp.add_bytes('q', "num_queries", b.num_lce_queries,
               "Number of LCE queries that are executed (default=1,000,000).");
  cp.add_bytes(
      "from", b.lce_from,
      "Use only lce queries which return at least 2^{from}  (default=0).");
  cp.add_bytes(
      "to", b.lce_to,
      "Use only lce queries which return up to 2^{to}-1 with (default=21)");

  // TODO: ADD ALGORITHMS
  cp.add_string(
      'a', "algorithm", b.algorithm,
      "LCE data structure "
      "that is computed: [u]ltra naive (default), [n]aive, "
      "prezza [m]ersenne, [p]rezza, or [s]tring synchronizing sets "
      "with tau = 512. [s2048], [s1024], [s512], [s256] for different tau "
      "values. Suffix _par for parallel sss, e.g. [s256_par]");

  if (!cp.process(argc, argv)) {
    std::exit(EXIT_FAILURE);
  }

  b.run<alx::lce::lce_naive<>>();
}