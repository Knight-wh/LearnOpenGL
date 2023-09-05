#默认使用目录名作为项目名
GET_DIR_NAME(DIRNAME)
set(TARGET_NAME ${DIRNAME})

# file(GLOB SRC
#     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
# )

# set(STR_TARGET_SOURCES "")
# foreach(SOURCE ${SRC})
# 	set(STR_TARGET_SOURCES "${STR_TARGET_SOURCES} ${SOURCE}")
# endforeach(SOURCE ${SRC})

# set(STR_TARGET_LIBS "Util")

# SETUP_PROJECT(${MODE} ${TARGET_NAME} ${STR_TARGET_SOURCES} ${STR_TARGET_LIBS})

# INSTALL_SHADERS()

add_executable(${TARGET_NAME} 1_height_map.cpp)
set(INSTALL_DIR "bin")
set_target_properties(${TARGET_NAME} PROPERTIES 
    FOLDER ${FOLDER_NAME}
)
target_link_libraries(${TARGET_NAME} Util)
install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_DIR})