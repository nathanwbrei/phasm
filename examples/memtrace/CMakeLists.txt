

message(STATUS "Including target 'phasm-example-memtrace'")
set(MEMTRACE_EXAMPLE_SOURCES vacuum_target.cpp)
add_executable(phasm-example-memtrace ${MEMTRACE_EXAMPLE_SOURCES})
target_compile_options(phasm-example-memtrace PUBLIC "-g" "-O0")
target_link_options(phasm-example-memtrace PUBLIC "-g")
set_target_properties(phasm-example-memtrace PROPERTIES EXPORT_COMPILE_COMMANDS On)
install(TARGETS phasm-example-memtrace DESTINATION bin)
