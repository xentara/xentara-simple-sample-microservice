cmake_minimum_required(VERSION 3.18)

project(xentara-simple-sample-microservice
	VERSION 1.0.0
	DESCRIPTION "A simple sample microservice for Xentara"
	HOMEPAGE_URL "https://www.xentara.io"
	LANGUAGES CXX)

# Force the use of C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Tell MSVC to set __cplusplus to the correct value
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options("/Zc:__cplusplus")
endif()

# Find the Xentara utility and plugin libraries
find_package(XentaraUtils REQUIRED)
find_package(XentaraPlugin REQUIRED)

# Add the plugin library target
add_library(
	${PROJECT_NAME} MODULE

	"src/Attributes.cpp"
	"src/Attributes.hpp"
	"src/Input.cpp"
	"src/Input.hpp"
	"src/Output.cpp"
	"src/Output.hpp"
	"src/Plugin.cpp"
	"src/Plugin.hpp"
	"src/ServiceProvider.cpp"
	"src/ServiceProvider.hpp"
	"src/SampleMicroservice.cpp"
	"src/SampleMicroservice.hpp"
)

# Link against the Xentara utility and plugin libraries
target_link_libraries(
	${PROJECT_NAME}

	PRIVATE
		Xentara::xentara-utils
		Xentara::xentara-plugin
)

# Add some defines
target_compile_definitions(
	${PROJECT_NAME}

	PRIVATE
		$<$<PLATFORM_ID:Windows>:NOMINMAX>
)

# Make output names adhere to Xentara convetions under Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(
		${PROJECT_NAME}
		
		PROPERTIES
			OUTPUT_NAME XentaraSimpleSampleMicroservice
			DEBUG_POSTFIX d
	)
endif()

# Generate the plugin manifest and add the plugin files to the install target
install_xentara_plugin(${PROJECT_NAME})
