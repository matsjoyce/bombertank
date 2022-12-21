configure_file("VCS.cpp.in" "VCS.cpp" @ONLY)
set(SOURCE
    TcpMessageSocket.cpp
    Constants.cpp
    MsgpackUtils.cpp
    ObjectTypeData.cpp
)

git_describe(GIT_NAME "--always")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/VCS.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/VCS.cpp" @ONLY)
list(APPEND SOURCE "${CMAKE_CURRENT_BINARY_DIR}/VCS.cpp")

add_library(bt_common STATIC ${SOURCE})
target_link_libraries(bt_common PRIVATE Qt6::Core Qt6::Network Qt6::Qml)
