add_executable(
  test_rolling_hash
  test_rolling_hash.cpp
)
target_link_libraries(
  test_rolling_hash
  GTest::gtest_main
  alx_rolling_hash
)

include(GoogleTest)
gtest_discover_tests(test_rolling_hash)
