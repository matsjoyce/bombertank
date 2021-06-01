set(SOURCE
    main.cpp
    GameServer.cpp
    Game.cpp
    GameMode.cpp
    objects/Base.cpp
    objects/Objects.cpp
    objects/Tank.cpp
    objects/Crate.cpp
    objects/Walls.cpp
    objects/Projectiles.cpp
)

add_executable(bt_server ${SOURCE})
target_link_libraries(bt_server PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network bt_common docopt box2d)
