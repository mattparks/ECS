file(GLOB_RECURSE TEST_HEADER_FILES "${CMAKE_SOURCE_DIR}/Test/*.hpp")
file(GLOB_RECURSE TEST_SOURCE_FILES "${CMAKE_SOURCE_DIR}/Test/*.cpp")
set(TEST_SOURCES
		${TEST_HEADER_FILES}
		${TEST_SOURCE_FILES}
		)

add_executable(ECS_Test ${TEST_SOURCES})

target_compile_features(ECS_Test PUBLIC cxx_std_17)
target_include_directories(ECS_Test PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(ECS_Test PRIVATE ECS)

set_target_properties(ECS_Test PROPERTIES
		FOLDER "ECS"
		)
