macro (perlsx_xml2)
  perlsx_find_package (LibXml2 "on ubuntu `sudo apt-get install libxml2-dev`" ${ARGN})
  if (LIBXML2_FOUND)
    include_directories (${LIBXML2_INCLUDE_DIR})
    add_definitions (${LIBXML2_DEFINITIONS})
    set (PERLSX_XML2 ${LIBXML2_LIBRARIES})
  endif ()
endmacro ()
