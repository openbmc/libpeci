cmake_minimum_required(VERSION 3.6)
project(libpeci)

add_library(peci SHARED peci.c)
configure_file(peci.pc.in peci.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/peci.pc DESTINATION lib/pkgconfig)

set(CMAKE_CXX_STANDARD 20)
set_property(TARGET peci PROPERTY C_STANDARD 99)
target_include_directories(peci PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(peci PROPERTIES VERSION "1.0" SOVERSION "1")

option(DBUS_RAW_PECI "Add the raw PECI D-Bus daemon to the build." OFF)

set(
  CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} \
  -Wall \
  -Wextra \
  -Wcast-align \
  -Wunused \
  -Wconversion \
  -Wsign-conversion \
  -Wnull-dereference \
  -Wdouble-promotion \
  -Wformat=2 \
  -Wno-unused-parameter \
  -Werror \
  -Wduplicated-cond \
  -Wduplicated-branches \
  -Wlogical-op \
")

install(TARGETS peci DESTINATION lib)
install(FILES peci.h DESTINATION include)

add_executable(peci_cmds peci_cmds.c)
add_dependencies(peci_cmds peci)
target_link_libraries(peci_cmds peci)

install(TARGETS peci_cmds
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

if(${DBUS_RAW_PECI})
  set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    -Wall \
    -Wextra \
    -Wcast-align \
    -Wunused \
    -Wconversion \
    -Wsign-conversion \
    -Wdouble-promotion \
    -Wformat=2 \
    -Wno-unused-parameter \
    -Werror \
    -Wduplicated-cond \
    -Wduplicated-branches \
    -Wlogical-op \
    -Wno-psabi \
    -fno-rtti \
  ")
  # This flag doesn't compile, but should be debuged in a future patchset
  #-Wnull-dereference \

  add_executable(raw-peci dbus_raw_peci.cpp)
  add_dependencies(raw-peci peci)

  find_package(Boost 1.73 REQUIRED)
  include_directories(${BOOST_SRC_DIR})

  add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
  add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED)
  add_definitions(-DBOOST_ALL_NO_LIB)
  add_definitions(-DBOOST_NO_RTTI)
  add_definitions(-DBOOST_NO_TYPEID)
  add_definitions(-DBOOST_EXCEPTION_DISABLE)
  add_definitions(-DBOOST_ASIO_DISABLE_THREADS)
  add_definitions(-DBOOST_ASIO_NO_DEPRECATED)

  target_link_libraries(raw-peci peci -lsystemd sdbusplus)

  install(TARGETS raw-peci
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib/static)

  find_package (PkgConfig REQUIRED)
  pkg_get_variable(SYSTEMD_SYSTEM_UNIT_DIR systemd systemdsystemunitdir)

  set(SERVICE_FILES ${PROJECT_SOURCE_DIR}/service_files/com.intel.peci.service)

  install(FILES ${SERVICE_FILES} DESTINATION "${SYSTEMD_SYSTEM_UNIT_DIR}")
endif()

