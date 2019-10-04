file(GLOB_RECURSE ECS_SOURCES "${CMAKE_SOURCE_DIR}/Sources/*.hpp" "${CMAKE_SOURCE_DIR}/Sources/*.cpp")

add_library(ECS ${ECS_SOURCES})

# Generates exports for shared builds
include(GenerateExportHeader)
generate_export_header(ECS)

target_compile_features(ECS PUBLIC cxx_std_17)
target_include_directories(ECS PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		)

set_target_properties(ECS PROPERTIES
		FOLDER "ECS"
		)
