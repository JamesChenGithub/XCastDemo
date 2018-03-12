cmake_minimum_required(VERSION 3.6)

PROJECT(xcast-dev)

include_directories(./../ 
                    ./../include
                    ./../thirdpart/jansson/include)
link_directories(../thirdpart)


set(RESOURCES ${PROJECT_SOURCE_DIR}/xcast-dev.rc)

set(xcast_dev_SOURCES
    main-view.h
    Resource.h
    stdafx.h
    targetver.h
    xcast-dev.h
    xcast-ui-handler.h
    main-view.cpp
    tab-view.cpp
    tree-view.cpp
    xcast-dev.cpp
    xcast-ui-handler.cc
    ${RESOURCES})


SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /MACHINE:X86") 
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "My multi config types" FORCE)

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")  

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /O2 /DNDEBUG /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /Od /D_DEBUG /Zi")

add_definitions(-D_UNICODE
                -DUNICODE
                -DWIN32
                -D_WINDOWS
                -D_CRT_SECURE_NO_WARNINGS)

add_library(xcast SHARED IMPORTED)
set_target_properties(xcast PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../libs/xcast.dll)
set_target_properties(xcast PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/../libs/xcast.lib)

add_executable(xcast_dev ${xcast_dev_SOURCES})
set_target_properties(xcast_dev PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY
                      ${PROJECT_SOURCE_DIR}/../bin)
#set xcast_dev as default startup project.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY 
             VS_STARTUP_PROJECT xcast_dev)
set(ENVIRONMENT_VARIABLES "../libs;" )
configure_file(template.vcxproj.user ${PROJECT_SOURCE_DIR}/xcast_dev.vcxproj.user @ONLY)
                                            
target_link_libraries(xcast_dev
PUBLIC
     xcast
     jansson
     kernel32
     user32
     gdi32
     winspool
     comdlg32
     advapi32
     shell32
     ole32
     oleaut32
     uuid
     odbc32
     odbccp32
     Msimg32)