

message(STATUS "Including target 'phasm-surrogate'")

set(SURROGATE_LIBRARY_SOURCES
        src/surrogate.cpp
        src/model.cpp
        src/surrogate_builder.cpp
        src/tensor.cpp
        src/plugin_loader.cc
        src/flamegraph.cpp
        src/omnitensor.cpp
        )

add_library(phasm-surrogate STATIC ${SURROGATE_LIBRARY_SOURCES})

target_compile_definitions(phasm-surrogate PRIVATE PHASM_PLUGIN_DIR=${CMAKE_INSTALL_PREFIX}/plugins)
# We pass PHASM_PLUGIN_DIR as a preprocessor arg so that PHASM can intelligently guess where
# to find plugins without requiring the user to set $PHASM_PLUGIN_DIR environment variable all the time.
# Note that the environment variable always takes precedence.

target_include_directories(phasm-surrogate
        PUBLIC include ../memtrace/include)

target_link_libraries(phasm-surrogate ${CMAKE_DL_LIBS})
install(TARGETS phasm-surrogate DESTINATION lib)


set(SURROGATE_LIBRARY_TEST_SOURCES
        test/capturing_tests.cpp
        test/sampling_tests.cpp
        test/sampling_tests.cpp
        test/range_tests.cpp
        test/memorizing_tests.cpp
        test/prism_tests.cpp
        test/optics_tests.cpp
        test/fluent_tests.cpp
        test/tutorial_tests.cpp
        test/plugin_tests.cc
        test/optics_oop_tests.cpp
        test/flamegraph_tests.cpp
        test/omnitensor_tests.cpp
        )
add_executable("phasm-surrogate-tests" ${SURROGATE_LIBRARY_TEST_SOURCES})
target_include_directories(phasm-surrogate-tests PRIVATE include ../memtrace/include)
target_link_libraries(phasm-surrogate-tests phasm-surrogate)


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include/phasm)

