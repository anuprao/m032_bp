# Author: Anup Jayapal Rao, Copyright 2024
# Ref:  https://github.com/jobroe/cmake-arm-embedded/blob/master/toolchain-arm-none-eabi.cmake

cmake_minimum_required(VERSION 3.13)

#set(CMAKE_VERBOSE_MAKEFILE ON)

# Project name
project(hello 
	VERSION 1.0
	DESCRIPTION "Very nice project"
	LANGUAGES ASM C CXX)

# Target definition
set(CMAKE_SYSTEM_NAME  Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Toolchain definition
set(TOOLCHAIN arm-none-eabi)

if(NOT DEFINED TOOLCHAIN_PREFIX)
	if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
		set(TOOLCHAIN_PREFIX "/usr")
	elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
		set(TOOLCHAIN_PREFIX "/usr/local")
	elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
		message(STATUS "Please specify the TOOLCHAIN_PREFIX !\n For example: -DTOOLCHAIN_PREFIX=\"C:/Program Files/GNU Tools ARM Embedded\" ")
	else()
		set(TOOLCHAIN_PREFIX "/usr")
		message(STATUS "No TOOLCHAIN_PREFIX specified, using default: " ${TOOLCHAIN_PREFIX})
	endif()
endif()

set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin)
set(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/include)
set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/lib)

set(HEX_EXT ".hex" )
set(BIN_EXT ".bin" )

# Set system depended extensions
if(WIN32)
	set(TOOLCHAIN_EXT ".exe" )
	set(BINARY_EXT ".exe" )
	set(DYNAMIC_LIBRARY_EXT ".dll" )
	set(STATIC_LIBRARY_EXT ".lib" )
else()
	set(TOOLCHAIN_EXT "" )
	set(BINARY_EXT ".elf" )
	set(DYNAMIC_LIBRARY_EXT ".so" )
	set(STATIC_LIBRARY_EXT ".a" )	
endif()

set(BUILD_BINARY "${CMAKE_PROJECT_NAME}${BINARY_EXT}")
set(BUILD_HEX "${CMAKE_PROJECT_NAME}${HEX_EXT}")
set(BUILD_BIN "${CMAKE_PROJECT_NAME}${BIN_EXT}")
	
# Perform compiler test with static library
#set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

option(BUILD_PCEMU "Build PC Emulation" OFF)
message(STATUS "The value of BUILD_PCEMU is: ${BUILD_PCEMU}")

# Set compiler/linker flags
set(OBJECT_GEN_FLAGS "-O0 -mtune=cortex-m0 -mthumb -march=armv6-m -fno-builtin -Wall -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs")

set(CMAKE_C_FLAGS   "${OBJECT_GEN_FLAGS} -std=gnu99 " CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "${OBJECT_GEN_FLAGS} -std=c++11 " CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS "${OBJECT_GEN_FLAGS} -x assembler-with-cpp " CACHE INTERNAL "ASM Compiler options")

set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/lds/gcc_arm_64k.ld -Wl,--gc-sections -nostartfiles -mtune=cortex-m0 -mthumb -march=armv6-m -mabi=aapcs -Wl,-Map=${CMAKE_PROJECT_NAME}.map" CACHE INTERNAL "Linker options")

# Set debug/release build configuration Options
set(CMAKE_C_FLAGS_DEBUG "-Og -g -fdiagnostics-color" CACHE INTERNAL "C Compiler options for debug build type")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g" CACHE INTERNAL "C++ Compiler options for debug build type")
set(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "ASM Compiler options for debug build type")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "Linker options for debug build type")

# Options for RELEASE build
set(CMAKE_C_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "C Compiler options for release build type")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "C++ Compiler options for release build type")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "ASM Compiler options for release build type")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto" CACHE INTERNAL "Linker options for release build type")

# Set compilers
set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-g++${TOOLCHAIN_EXT} CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "ASM Compiler")
set(CMAKE_SIZE_UTIL ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-size${TOOLCHAIN_EXT} CACHE INTERNAL "Size tool")
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objcopy${TOOLCHAIN_EXT} CACHE INTERNAL "ObjCopy tool")

set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/${${TOOLCHAIN}} ${CMAKE_PREFIX_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Add sub directories
add_subdirectory(thirdparty/m032) 
add_subdirectory(thirdparty/std_driver) 
add_subdirectory(thirdparty/rtos) 

#message(STATUS "The value of rtos_SOURCES is: ${rtos}")

#add_subdirectory(drv/hw)
#add_subdirectory(drv/pcemu)
#add_subdirectory(srv) 
#add_subdirectory(app) 

file(GLOB SOURCES_ASM asm/*.s)
set(SOURCES ${SOURCES_ASM})

set(PATH_INCLUDES inc)
file(GLOB SOURCES_MAIN src/*.c)
list (APPEND SOURCES ${SOURCES_MAIN})

file(GLOB SOURCES_drv_hw ${CMAKE_CURRENT_SOURCE_DIR}/drv/hw/src/*.c)
file(GLOB SOURCES_drv_pcemu ${CMAKE_CURRENT_SOURCE_DIR}/drv/pcemu/src/*.c)

if(BUILD_PCEMU)
	list (APPEND PATH_INCLUDES drv_pcemu/inc)
	list (APPEND SOURCES ${SOURCES_drv_pcemu})
else()
	list (APPEND PATH_INCLUDES drv_hw/inc)
	list (APPEND SOURCES ${SOURCES_drv_hw})
endif()

list (APPEND PATH_INCLUDES srv/inc)
file(GLOB SOURCES_srv ${CMAKE_CURRENT_SOURCE_DIR}/srv/src/*.c)
list (APPEND SOURCES ${SOURCES_srv})

list (APPEND PATH_INCLUDES app/inc)
file(GLOB SOURCES_app ${CMAKE_CURRENT_SOURCE_DIR}/app/src/*.c)
list (APPEND SOURCES ${SOURCES_app})

add_executable(${BUILD_BINARY} ${SOURCES})

#####

target_include_directories(${BUILD_BINARY} 
	PUBLIC
		${PATH_INCLUDES}
	)

target_link_libraries(${BUILD_BINARY}
	m032
	std_driver
	rtos
)

#####

add_custom_command(TARGET ${BUILD_BINARY} POST_BUILD
	COMMAND ${CMAKE_SIZE_UTIL} --format=berkeley ${BUILD_BINARY}
	COMMENT "Analysing ${BUILD_BINARY}"
)

add_custom_command(TARGET ${BUILD_BINARY} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${BUILD_HEX}
	COMMENT "Generating ${BUILD_HEX}"
	)

add_custom_command(TARGET ${BUILD_BINARY} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BUILD_BIN}
	COMMENT "Generating ${BUILD_BIN}"
	)
