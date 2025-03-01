check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/net-builder-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME net-builder
    SOURCE_FILES model/net-builder.cc
                 helper/net-builder-helper.cc
    HEADER_FILES model/net-builder.h
                 helper/net-builder-helper.h
    LIBRARIES_TO_LINK ${libcore}
    TEST_SOURCES test/net-builder-test-suite.cc
                 ${examples_as_tests_sources}
)
