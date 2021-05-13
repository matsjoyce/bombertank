set(SOURCE
    TcpMessageSocket.cpp
)

add_library(bt_common STATIC ${SOURCE})
target_link_libraries(bt_common PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)