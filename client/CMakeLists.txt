set(SOURCE
    main.cpp
    qml.qrc
    AppContext.cpp
    GameServer.cpp
    GameState.cpp
    MapView.cpp
    objects/Base.cpp
    objects/TankControl.cpp
)

add_executable(bt_client ${SOURCE})
target_link_libraries(
    bt_client 
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    bt_common
)
add_dependencies(bt_client bt_server)