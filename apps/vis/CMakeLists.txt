############################################################
# CMake Build Script for the vis executable

link_directories(${SCHISM_LIBRARY_DIRS})

include_directories(
  ${SCHISM_INCLUDE_DIRS}
  ${REND_INCLUDE_DIR}
  ${PROV_INCLUDE_DIR}
  ${COMMON_INCLUDE_DIR}
  ${LAMURE_CONFIG_DIR}
  ${VT_INCLUDE_DIR}
  ${PVS_COMMON_INCLUDE_DIR}
  ${FREEIMAGE_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${GLFW3_INCLUDE_DIR}
  ${OpenGL_INCLUDE_DIRS}
)

link_directories(${OpenGL_LIBRARY_DIRS})

include_directories(SYSTEM 
  ${SCHISM_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR})


InitApp(${CMAKE_PROJECT_NAME}_vis)

############################################################
# Libraries
target_link_libraries(${PROJECT_NAME}
  ${PROJECT_LIBS}
  ${REND_LIBRARY}
  ${PVS_COMMON_LIBRARY}
  ${PROV_LIBRARY}
  ${VT_LIBRARY}
  ${OpenGL_LIBRARY}
  ${GLEW_LIBRARY}
  optimized ${GLFW3_LIBRARY} debug ${GLFW3_LIBRARY_DEBUG}
  ${OpenGL_LIBRARIES}
  ${OPENGL_LIBRARIES}
  optimized ${SCHISM_CORE_LIBRARY} debug ${SCHISM_CORE_LIBRARY_DEBUG}
  optimized ${SCHISM_GL_CORE_LIBRARY} debug ${SCHISM_GL_CORE_LIBRARY_DEBUG}
  optimized ${SCHISM_GL_UTIL_LIBRARY} debug ${SCHISM_GL_UTIL_LIBRARY_DEBUG}
  optimized ${Boost_REGEX_LIBRARY_RELEASE} debug ${Boost_REGEX_LIBRARY_DEBUG}
)

message("SCHISM_CORE_LIBRARY: ${SCHISM_CORE_LIBRARY}")
message("SCHISM_GL_CORE_LIBRARY: ${SCHISM_GL_CORE_LIBRARY}")
message("SCHISM_GL_UTIL_LIBRARY: ${SCHISM_GL_UTIL_LIBRARY}")
message("GLFW3_LIBRARY: ${GLFW3_LIBRARY}")
message("GLFW3_LIBRARY_DEBUG: ${GLFW3_LIBRARY_DEBUG}")
message("OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")

