add_executable(
  test_pred
  test_pred.cpp
)
target_link_libraries(
  test_pred
  GTest::gtest_main
  alx_pred
)

include(GoogleTest)
gtest_discover_tests(test_pred)
