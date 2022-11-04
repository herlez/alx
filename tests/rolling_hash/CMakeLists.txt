add_executable(
  test_rolling_hash
  test_rolling_hash.cpp
)
target_link_libraries(
  test_rolling_hash
  GTest::gtest_main
  alx_rolling_hash
)

add_executable(
  test_string_synchronizing_set
  test_string_synchronizing_set.cpp
)
target_link_libraries(
  test_string_synchronizing_set
  GTest::gtest_main
  alx_string_synchronizing_set
  alx_pred_index
  libsais
  fmt::fmt-header-only
)

include(GoogleTest)
gtest_discover_tests(test_rolling_hash)
gtest_discover_tests(test_string_synchronizing_set)
