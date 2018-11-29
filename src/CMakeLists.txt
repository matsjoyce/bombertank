set(SOURCE
    point.cpp
    rect.cpp
    object.cpp
    map.cpp
    rendermap.cpp
    eventqueue.cpp
    effect.cpp
    signal.cpp
    stagemanager.cpp
    objects/loader.cpp
    objects/walls.cpp
    objects/player.cpp
    objects/playeritem.cpp
    objects/bomb.cpp
    objects/chest.cpp
    objects/attackutils.cpp
)

add_library(bombertank STATIC ${SOURCE})

add_executable(main main.cpp)
target_link_libraries(main bombertank sfml-graphics sfml-audio stdc++fs pthread)

add_executable(maped maped.cpp)
target_link_libraries(maped bombertank sfml-graphics sfml-audio stdc++fs pthread)
