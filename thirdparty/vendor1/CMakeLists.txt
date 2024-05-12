# Set the project name
project (vendor1)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

#message(STATUS "The value of SOURCES is: ${SOURCES}")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
#add_library(sub_${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	INTERFACE
		${PROJECT_SOURCE_DIR}/inc
)

