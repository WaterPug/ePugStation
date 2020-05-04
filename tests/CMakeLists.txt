find_package(Catch2 CONFIG REQUIRED)

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PRIVATE Catch2::Catch2)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE project_warnings catch_main Catch2::Catch2)

include(Catch)

# Automatically discover tests defined in catch test files.
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  EXTRA_ARGS
  -s
  --reporter=xml
  --out=tests.xml)