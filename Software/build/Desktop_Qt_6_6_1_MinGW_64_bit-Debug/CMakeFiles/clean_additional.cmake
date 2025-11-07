# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\anzeigetafel_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\anzeigetafel_autogen.dir\\ParseCache.txt"
  "anzeigetafel_autogen"
  )
endif()
