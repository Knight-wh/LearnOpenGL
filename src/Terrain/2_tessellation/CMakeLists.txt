#默认使用目录名作为项目名
GET_DIR_NAME(DIRNAME)
set(TARGET_NAME ${DIRNAME})

add_executable(${TARGET_NAME} 2_tessellation.cpp)
set_target_properties(${TARGET_NAME} PROPERTIES 
  FOLDER ${FOLDER_NAME}
)
target_link_libraries(${TARGET_NAME} Util)
set(INSTALL_DIR "bin")
install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_DIR})