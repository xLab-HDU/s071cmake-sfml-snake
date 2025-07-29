cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 选项：是否将SFML库构建为共享库，动态模式设为 ON；静态模式则设为 OFF
option(BUILD_SHARED_LIBS "Build SFML libraries as shared" OFF)

# 查找本地安装的 SFML 开发包
# set(SFML_STATIC_LIBRARIES TRUE) # 配合上面的 BUILD_SHARED_LIBS OFF 可实现静态链接
# find_package(SFML COMPONENTS Graphics Window System Audio Network REQUIRED)

# 获取SFML源码并构建
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network)
       
# 针对MinGW，需要将 libstdc++ 和 libgcc 编译进可执行文件中，避免运行时依赖外部 DLL
# target_link_libraries(main PRIVATE
#     -static-libgcc
#     -static-libstdc++
# )

# 测试 & 打包
enable_testing()
add_test(NAME SmokeTest COMMAND main)

install(TARGETS main RUNTIME DESTINATION bin)
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