check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/central-controller-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME central-controller
    SOURCE_FILES model/central-controller.cc
                 helper/central-controller-helper.cc
    HEADER_FILES model/central-controller.h
                 helper/central-controller-helper.h
    LIBRARIES_TO_LINK ${libcore}
    TEST_SOURCES test/central-controller-test-suite.cc
                 ${examples_as_tests_sources}
)
