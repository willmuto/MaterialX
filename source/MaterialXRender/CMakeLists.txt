include_directories(    
    ${EXTERNAL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

file(GLOB_RECURSE materialx_source "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE materialx_header "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

if(APPLE)
	find_library(COCOA_FRAMEWORK Cocoa)
	find_package(OpenGL REQUIRED)
	file(GLOB_RECURSE materialx_source_oc "${CMAKE_CURRENT_SOURCE_DIR}/*.m")
	message("Objective C files: " ${materialx_source_oc})
	set_source_files_properties(${materialx_source_oc} PROPERTIES 
								COMPILE_FLAGS "-x objective-c++")
	set(materialx_source ${materialx_source} ${materialx_source_oc})
elseif(UNIX)
	find_package(PkgConfig REQUIRED)
	find_package(X11 REQUIRED)
	if (NOT X11_FOUND)
		message("Error in building MaterialXRender: X11 was not found")
	endif()
	if (NOT X11_Xt_FOUND)
		message("Error in building MaterialXRender: Xt was not found")
	endif()

	find_package(OpenGL REQUIRED)
	if (NOT OPENGL_FOUND)
		message("Error in building MaterialXRender: OpenGL was not found")
	endif()

	include_directories(z{X11_INCLUDE_DIR})
endif()

function(assign_source_group prefix)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${prefix}\\${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group("Header Files" ${materialx_header})
assign_source_group("Source Files" ${materialx_source})

add_library(MaterialXRender STATIC 
    ${materialx_source} 
    ${materialx_header}
)

if(MSVC)
    target_link_libraries(
        MaterialXRender
        MaterialXGenShader
        MaterialXCore
        Opengl32
        ${CMAKE_DL_LIBS})
elseif(APPLE)
    target_link_libraries(
        MaterialXRender
        MaterialXGenShader
        MaterialXCore
        ${CMAKE_DL_LIBS}
        ${OPENGL_LIBRARIES}
	"-framework Foundation"
	"-framework Cocoa"
)
elseif(UNIX)
    target_link_libraries(
        MaterialXRender
        MaterialXGenShader
        MaterialXCore
        ${CMAKE_DL_LIBS}
        ${OPENGL_LIBRARIES}
		${X11_LIBRARIES}
        ${X11_Xt_LIB}
    )
endif()

if(MATERIALX_BUILD_OPENIMAGEIO)
	set(OPENIMAGEIO_ROOT_DIR ${MATERIALX_OPENIMAGEIO_DIR})
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/External/OpenImageIO")
	find_package(OpenImageIO REQUIRED)
	if(OPENIMAGEIO_FOUND)
		include_directories(${OPENIMAGEIO_INCLUDE_DIR})
		target_link_libraries(MaterialXRender ${OPENIMAGEIO_LIBRARIES})
		add_definitions(-DMATERIALX_BUILD_OPENIMAGEIO)
	endif()
endif()

set_target_properties(
    MaterialXRender PROPERTIES
    OUTPUT_NAME MaterialXRender
    COMPILE_FLAGS "${EXTERNAL_COMPILE_FLAGS}"
    LINK_FLAGS "${EXTERNAL_LINK_FLAGS}"
    VERSION "${MATERIALX_LIBRARY_VERSION}"
    SOVERSION "${MATERIALX_MAJOR_VERSION}")

install(TARGETS MaterialXRender
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MaterialXRender/
    FILES_MATCHING PATTERN "*.h*")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/MaterialXRender.pdb"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/" OPTIONAL)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../documents/Examples" DESTINATION ${CMAKE_INSTALL_PREFIX}/documents)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../documents/Libraries" DESTINATION ${CMAKE_INSTALL_PREFIX}/documents)
