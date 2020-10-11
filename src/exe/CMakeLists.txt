# Set the project name
project (lstmsdparser C CXX)

file(GLOB_RECURSE src
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/*.c
	${CMAKE_CURRENT_LIST_DIR}/*.cc
	${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

# Add an executable for testing purpose
add_executable(lstmsdparser ${src})

target_link_libraries(lstmsdparser
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC dynet
    PUBLIC lstmsdparser-lib
)