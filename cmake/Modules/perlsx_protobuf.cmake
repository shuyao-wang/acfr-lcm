macro (perlsx_protobuf)
  perlsx_find_package (Protobuf "on ubuntu `sudo apt-get install protobuf-compiler python-protobuf`" ${ARGN})
  if (PROTOBUF_FOUND)
    include_directories (${PROTOBUF_INCLUDE_DIRS})
    set (PERLSX_PROTOBUF ${PROTOBUF_LIBRARIES})
  endif ()
endmacro ()
