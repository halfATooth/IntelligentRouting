build_lib_example(
    NAME net-builder-example
    SOURCE_FILES net-builder-example.cc
    LIBRARIES_TO_LINK 
      ${libnet-builder}
      ${libcore}
      ${libpoint-to-point}
      ${libinternet}
      ${libapplications}
      ${libflow-monitor}
      ${libcentral-controller}
)
