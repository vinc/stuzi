project (stuzi)
cmake_minimum_required (VERSION 2.6) 

find_package (CURL REQUIRED)
include_directories (${CURL_INCLUDE_DIR})

add_executable (stuzi src/curl.c src/records.c src/main.c)
target_link_libraries (stuzi ${CURL_LIBRARY})
install (TARGETS stuzi RUNTIME DESTINATION bin)
