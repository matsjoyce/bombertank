configure_file("VCS.cpp.in" "VCS.cpp" @ONLY)
set(SOURCE
    TcpMessageSocket.cpp
    Constants.cpp
    MsgpackUtils.cpp
    ObjectTypeData.cpp
)

git_describe(GIT_NAME "--always")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/VCS.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/VCS.cpp" @ONLY)
set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/bt_commonplugin_init.cpp" PROPERTY SKIP_AUTOGEN ON)
list(APPEND SOURCE "${CMAKE_CURRENT_BINARY_DIR}/VCS.cpp")

add_library(bt_common STATIC ${SOURCE})
qt_add_qml_module(bt_common
    URI BT.Common
    VERSION 1.0
)
qt_add_resources(bt_common "data_files"
    PREFIX "/"
    FILES
        ../data/objects.json
        ../data/tank_modules.json)
target_link_libraries(bt_common PRIVATE Qt6::Core Qt6::Network Qt6::Qml msgpackc-cxx)
