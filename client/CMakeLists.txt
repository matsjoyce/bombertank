set(SOURCE
    main.cpp
    qml.qrc
    AppContext.cpp
    GameServer.cpp
    GameState.cpp
    MapView.cpp
    objects/Base.cpp
    objects/TankControl.cpp
    objects/TankState.cpp
    objects/TurretState.cpp
)

add_executable(bt_client ${SOURCE})
target_link_libraries(
    bt_client
    PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets # For file dialog support
    Qt6::Qml
    bt_common
    docopt
)
qt_add_qml_module(bt_client
    URI BT
    VERSION 1.0
)
# HACK
target_include_directories(bt_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/objects)
add_dependencies(bt_client bt_server)
