set(SOURCE
    TcpMessageSocket.cpp
    Constants.cpp
)

add_library(bt_common STATIC ${SOURCE})
target_link_libraries(bt_common PRIVATE Qt6::Core Qt6::Network Qt6::Qml)
