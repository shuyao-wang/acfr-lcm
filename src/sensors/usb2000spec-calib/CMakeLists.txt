perls_add_executable (usb2000-spec-calib main.c USB2000Lib.c)
perls_target_link_libraries (usb2000-spec-calib perls-lcmtypes perls-common)
