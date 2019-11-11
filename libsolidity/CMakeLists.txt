add_definitions(-Ddev=dev_SOLIDITY)

set(SOLIDITY_SOURCE_DIR ${CMAKE_SOURCE_DIR}/solidity)
set(SOLIDITY_INCLUDE_DIR ${SOLIDITY_SOURCE_DIR} ${SOLIDITY_SOURCE_DIR}/build/include ${SOLIDITY_SOURCE_DIR}/build/include/solidity)
message("SOLIDITY_SOURCE_DIR=${SOLIDITY_SOURCE_DIR},SOLIDITY_INCLUDE_DIR=${SOLIDITY_INCLUDE_DIR}") 

file(GLOB_RECURSE SOLIDITY_SOL_sources "${SOLIDITY_SOURCE_DIR}/*.cpp") 
file(GLOB_RECURSE SOLIDITY_SOL_headers "${SOLIDITY_SOURCE_DIR}/*.h")

find_package(Z3 QUIET) 
if (${Z3_FOUND}) 
	include_directories(${Z3_INCLUDE_DIR})
	add_definitions(-DHAVE_Z3)
	message("Z3 SMT solver FOUND.") 
else() 
	message("Z3 SMT solver NOT found.")
	list(REMOVE_ITEM SOLIDITY_SOL_sources "${SOLIDITY_SOURCE_DIR}/libsolidity/formal/Z3Interface.cpp")
	list(REMOVE_ITEM SOLIDITY_SOL_sources "${SOLIDITY_SOURCE_DIR}/libsolidity/formal/Z3CHCInterface.cpp") 
endif()

find_package(CVC4 QUIET) 
if (${CVC4_FOUND}) 
	add_definitions(-DHAVE_CVC4)
	message("CVC4 SMT solver found. This enables optional SMT checking with CVC4.") 
else() 
	message("CVC4 SMT solver NOT found.")
	list(REMOVE_ITEM SOLIDITY_SOL_sources "${SOLIDITY_SOURCE_DIR}/libsolidity/formal/CVC4Interface.cpp") 
endif()

add_library(solidity ${SOLIDITY_SOL_sources} ${SOLIDITY_SOL_headers}) 
# message("SOLIDITY_SOL_sources=${SOLIDITY_SOL_sources},SOLIDITY_SOL_headers=${SOLIDITY_SOL_headers},CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")

# get_target_property(SOL_INCLUDES solidity INCLUDE_DIRECTORIES)
# message("get_target_property SOL_INCLUDES=${SOL_INCLUDES},INCLUDE_DIRECTORIES=${INCLUDE_DIRECTORIES},CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}") 
# list(REMOVE_ITEM SOL_INCLUDES ${CMAKE_SOURCE_DIR}) 
# message("SOL_INCLUDES=${SOL_INCLUDES},INCLUDE_DIRECTORIES=${INCLUDE_DIRECTORIES}") 
set_target_properties(solidity PROPERTIES INCLUDE_DIRECTORIES "")

target_include_directories(solidity PRIVATE ${SOLIDITY_INCLUDE_DIR}) 
target_include_directories(solidity SYSTEM PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(solidity ${Boost_REGEX_LIBRARIES} jsoncpp)

file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(contract ${SRC_LIST} ${HEADERS})

set_target_properties(contract PROPERTIES INCLUDE_DIRECTORIES "")
target_include_directories(contract PRIVATE ${SOLIDITY_INCLUDE_DIR})
target_include_directories(contract PRIVATE ${Boost_INCLUDE_DIR})

target_link_libraries(contract solidity)

message("Boost_PROGRAM_OPTIONS_LIBRARIES=${Boost_PROGRAM_OPTIONS_LIBRARIES},Boost_REGEX_LIBRARIES=${Boost_REGEX_LIBRARIES},Boost_FILESYSTEM_LIBRARIES=${Boost_FILESYSTEM_LIBRARIES},Boost_SYSTEM_LIBRARIES=${Boost_SYSTEM_LIBRARIES}") 
target_link_libraries(contract ${Boost_PROGRAM_OPTIONS_LIBRARIES}) 
target_link_libraries(contract ${Boost_REGEX_LIBRARIES})
target_link_libraries(contract ${Boost_FILESYSTEM_LIBRARIES})
target_link_libraries(contract ${Boost_SYSTEM_LIBRARIES})
target_link_libraries(contract jsoncpp_lib_static)

if (${Z3_FOUND}) 
	target_link_libraries(contract z3)
endif()

if (${CVC4_FOUND})
	target_link_libraries(solidity PUBLIC CVC4::CVC4) 
endif() 