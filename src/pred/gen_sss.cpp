/*******************************************************************************
 * src/pred/gen_sss.cpp
 *
 * Copyright (C) 2022 Alexander Herlez <alexander.herlez@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <tlx/cmdline_parser.hpp>

#include "rolling_hash/string_synchronizing_set.hpp"
#include "util/io.hpp"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
  std::vector<std::string> algorithms{"all", "sss256", "sss512", "sss1024",
                                      "sss2048"};

  std::filesystem::path text_path;
  std::vector<uint8_t> text;
  std::string algorithm{"sss512"};

  tlx::CmdlineParser cp;
  cp.set_description(
      "This program writes the string synchronizing set of a text to disk. "
      "This can be used as input for predecessor data structures.");
  cp.set_author("Alexander Herlez <alexander.herlez@tu-dortmund.de>");

  cp.add_param_path("text_path", text_path, "The path to the text.");
  cp.add_string(
      'a', "algorithm", algorithm,
      fmt::format("Name of data structure which is benchmarked. Options: {}",
                  algorithms));
  if (!cp.process(argc, argv)) {
    std::exit(EXIT_FAILURE);
  }

  // Check parameters
  {
    if (!fs::is_regular_file(text_path) || fs::file_size(text_path) == 0) {
      fmt::print("Text file {} is empty or does not exist.\n",
                 text_path.string());
      return -1;
    }
    if (std::find(algorithms.begin(), algorithms.end(), algorithm) ==
        algorithms.end()) {
      fmt::print("Algorithm {} is not specified.\n Use one of {}\n", algorithm,
                 algorithms);
      return -1;
    }
  }

  text = alx::util::load_vector<uint8_t>(text_path);

  std::filesystem::path output_path = text_path;
  if (algorithm == "sss256" || algorithm == "all") {
    output_path += ".sss256";
    alx::rolling_hash::sss<uint64_t, 256> sss(text);
    alx::util::write_vector(output_path, sss.get_sss());
  }
  if (algorithm == "sss512" || algorithm == "all") {
    output_path += ".sss512";
    alx::rolling_hash::sss<uint64_t, 512> sss(text);
    alx::util::write_vector(output_path, sss.get_sss());
  }
  if (algorithm == "sss1024" || algorithm == "all") {
    output_path += ".sss1024";
    alx::rolling_hash::sss<uint64_t, 1024> sss(text);
    alx::util::write_vector(output_path, sss.get_sss());
  }
  if (algorithm == "sss2048" || algorithm == "all") {
    output_path += ".sss2048";
    alx::rolling_hash::sss<uint64_t, 2048> sss(text);
    alx::util::write_vector(output_path, sss.get_sss());
  }
  return 0;
}
