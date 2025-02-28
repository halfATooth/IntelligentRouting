check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/shared-memory-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME shared-memory
    SOURCE_FILES model/shared-memory.cc
                 helper/shared-memory-helper.cc
    HEADER_FILES model/shared-memory.h
                 helper/shared-memory-helper.h
    LIBRARIES_TO_LINK ${libcore}
    TEST_SOURCES test/shared-memory-test-suite.cc
                 ${examples_as_tests_sources}
)
