cmake_minimum_required(VERSION 3.21.0)

project(OpenGLDemo LANGUAGES C CXX VERSION 1.0.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################################################
# Set target platform type. 
################################################################################
if(UNIX)
    set(CURRENT_PLATFORM_SYSTEM "Unix")
elseif(WIN32)
    set(CURRENT_PLATFORM_SYSTEM "Windows")
else()
    set(CURRENT_PLATFORM_SYSTEM "unkonw")
endif()
message(STATUS "current platform system: ${CURRENT_PLATFORM_SYSTEM} ")

if(NOT ("${CURRENT_PLATFORM_SYSTEM}" STREQUAL "Windows" ))
	message(FATAL_ERROR "${CURRENT_PLATFORM_SYSTEM} system is not supported!")
endif()

################################################################################
# Set target arch type. 
################################################################################
if(CMAKE_CL_64)
    set(CURRENT_PLATFORM "x64")
else(CMAKE_CL_64)
    set(CURRENT_PLATFORM "x86")
endif(CMAKE_CL_64)
message(STATUS "Current Platform is ${CURRENT_PLATFORM}")

if(NOT ("${CURRENT_PLATFORM}" STREQUAL "x64" OR "${CURRENT_PLATFORM}" STREQUAL "x86"))
	message(FATAL_ERROR "${CURRENT_PLATFORM} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
	"Release"
)

################################################################################
# Source groups
################################################################################
set(Headers
    "CommonFileSrc/CameraWrapper.h"
    "CommonFileSrc/CommonDataDef.h"
    "CommonFileSrc/Singleton.h"
    "DataModel/Mesh.h"
    "DataModel/Model.h"
    "DataModel/VAOFactory.h"
    "ExerciceState/ExerciceState.h"
    "ExerciceState/StateSourceFactory.h"
    "RenderLoop.h"
    "Shader/Shader.h"
    "Shader/ShaderFactory.h"
    "third_lib/stb/stb_image.h"
)
#source_group("Headers" FILES ${Headers})

set(Sources
    "CommonFileSrc/CommonDataDef.cpp"
    "DataModel/Mesh.cpp"
    "DataModel/Model.cpp"
    "DataModel/VAOFactory.cpp"
    "ExerciceState/ExerciceState.cpp"
    "ExerciceState/StateSourceFactory.cpp"
    "main.cpp"
    "RenderLoop.cpp"
    "Shader/Shader.cpp"
    "Shader/ShaderFactory.cpp"
    "third_lib/GLAD/src/glad.c"
    "third_lib/stb/stb_image.cpp"
)
#source_group("Sources" FILES ${Sources})
set(ALL_FILES
	${Headers}
	${Sources}
)

################################################################################
# Source Group Macro
################################################################################
macro(source_group_by_dir source_files)
    if(MSVC)
        set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(sgbd_file ${${source_files}})
            string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC)
endmacro(source_group_by_dir)

source_group_by_dir(ALL_FILES)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} 
	${ALL_FILES}
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} 
PRIVATE
	ExerciceState
	DataModel
	CommonFileSrc
	third_lib/Assimp/include
	third_lib/glm
	Shader
	third_lib/stb
	third_lib/GLAD/include
	third_lib/GLFW/include
)

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
	glfw3
	assimp-vc141-mtd
)
target_link_libraries(${PROJECT_NAME} 
PRIVATE 
	"${ADDITIONAL_LIBRARY_DEPENDENCIES}"
)

target_link_directories(${PROJECT_NAME} 
PRIVATE
	third_lib/GLFW/lib
	third_lib/Assimp/lib
)

################################################################################
# Set target properties
################################################################################
if("${CURRENT_PLATFORM}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} 
	PROPERTIES
		#Output directory
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../ExeFile/Debug_64"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE   "${CMAKE_CURRENT_SOURCE_DIR}/../ExeFile/Release_64"
    )
else()
	set_target_properties(${PROJECT_NAME} 
	PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../ExeFile/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE   "${CMAKE_CURRENT_SOURCE_DIR}/../ExeFile/Release"
    )
endif()

#Common
set_target_properties(${PROJECT_NAME}
PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"	
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
	target_compile_options(${PROJECT_NAME} 
	PRIVATE
		$<$<CONFIG:Debug>:
			/Od
		>
		$<$<CONFIG:Release>:
			/O2;
			/Oi;
			/Gy
		>
		/permissive-;
		/sdl;
		/W3;
		${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
		${DEFAULT_CXX_EXCEPTION_HANDLING}
	)

	target_link_options(${PROJECT_NAME} 
	PRIVATE
		$<$<CONFIG:Debug>:
			/DEBUG;
			/SUBSYSTEM:CONSOLE
		>
		$<$<CONFIG:Release>:
			/SUBSYSTEM:WINDOWS;
			/ENTRY:mainCRTStartup
			#/ENTRY:WinMainCRTStartup
		>
		/OPT:REF;
		/OPT:ICF;
		/INCREMENTAL:NO 
	)
endif()

################################################################################
# Post build events
################################################################################
add_custom_command(TARGET ${PROJECT_NAME}
	POST_BUILD
	COMMAND  PostBuild_64.bat
	COMMENT "PostBuild_64.bat"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)