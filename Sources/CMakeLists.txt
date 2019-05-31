file(GLOB_RECURSE ECS_HEADER_FILES 
		"${CMAKE_SOURCE_DIR}/Sources/*.hpp" 
		"${CMAKE_SOURCE_DIR}/Sources/*.inl"
		)
file(GLOB_RECURSE ECS_SOURCE_FILES "${CMAKE_SOURCE_DIR}/Sources/*.cpp")
set(ECS_SOURCES
		${ECS_HEADER_FILES}
		${ECS_SOURCE_FILES}
		)

add_library(ECS ${ECS_SOURCES})

target_compile_features(ECS PUBLIC cxx_std_17)
target_include_directories(ECS PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		)

set_target_properties(ECS PROPERTIES
		FOLDER "ECS"
		)
