cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 选项：是否构建为共享库，共享库设为 ON；静态库设为 OFF
option(BUILD_SHARED_LIBS "Build libraries as shared" OFF)

# 查找本地安装的 SFML 开发包
# set(SFML_STATIC_LIBRARIES TRUE) # 配合上面的 BUILD_SHARED_LIBS OFF 可实现静态链接
# find_package(SFML COMPONENTS Graphics Window System Audio REQUIRED)

# 获取SFML源码并构建
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

if(WIN32)
    # Windows GUI 程序，如果想隐藏控制台窗口
    add_executable(main WIN32 src/main.cpp)
else()
    # Linux/macOS 或控制台程序
    add_executable(main src/main.cpp)
endif()
target_link_libraries(main PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)
target_compile_features(main PRIVATE cxx_std_17)

# 在Windows系统中（使用 MinGW-w64）构建时
# 静态链接需要将libstdc++、libgcc编译进可执行文件中，避免运行时依赖外部 DLL
if(WIN32 AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")))
  target_link_libraries(main PRIVATE
      -static-libgcc
      -static-libstdc++
    )  
endif()

# 使用-mwindows标志隐藏控制台窗口
# if (WIN32)
#   # target_link_options(main PRIVATE "-mwindows")
#   add_executable(main WIN32)
# endif()

# 复制data文件夹到可执行文件输出目录
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data # 源路径：项目根目录下的data文件夹
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data  # 目标路径：build/bin/所在目录下的data文件夹
    COMMENT "Copying data directory to output directory"
)

# 测试 & 打包
enable_testing()
add_test(NAME SmokeTest
         COMMAND main
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


install(TARGETS main RUNTIME DESTINATION .)
install(DIRECTORY "data/"
        DESTINATION "data"
        FILES_MATCHING PATTERN "*"
)

set(CPACK_PACKAGE_NAME "Snake")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "HDU")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CMake-SFML-Snake")
set(CPACK_PACKAGE_CONTACT "contact@example.com")
include(CPack)