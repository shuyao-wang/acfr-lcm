

perls_add_executable (evologics-usbl evologics_usbl.cpp )
perls_target_link_libraries (evologics-usbl perls-lcmtypes perls-common proj z pthread)

perls_add_executable (evologics-modem evologics_modem.cpp )
perls_target_link_libraries (evologics-modem perls-lcmtypes perls-common z pthread)

perls_add_executable (evologics-ahrs evologics_ahrs.cpp )
perls_target_link_libraries (evologics-ahrs perls-lcmtypes perls-common)

perls_add_executable (evologics-extended evologics_extended.cpp )
perls_target_link_libraries (evologics-extended perls-lcmtypes perls-common z pthread)

perls_add_executable (evologics-combined evologics_combined.cpp )
perls_target_link_libraries (evologics-combined perls-lcmtypes acfr-common z pthread)
