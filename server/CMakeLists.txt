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
    objects/Turret.cpp
    objects/actions/TankModule.cpp
    objects/actions/MainGun.cpp
    objects/actions/MachineGun.cpp
    objects/actions/RocketLauncher.cpp
    objects/actions/StatBoostModules.cpp
)

add_executable(bt_server ${SOURCE})
target_link_libraries(bt_server PRIVATE Qt6::Core Qt6::Network bt_common docopt box2d Qt6::Qml)
