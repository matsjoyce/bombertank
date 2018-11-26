set(SOURCE
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
    objects/bomb.cpp
    objects/chest.cpp
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    message.proto
    generic_msg.proto
)

add_library(bombertank STATIC ${SOURCE} ${PROTO_SRCS})

add_executable(main main.cpp)
target_link_libraries(main bombertank sfml-graphics sfml-audio stdc++fs ${Protobuf_LIBRARIES})

add_executable(maped maped.cpp)
target_link_libraries(maped bombertank sfml-graphics sfml-audio stdc++fs ${Protobuf_LIBRARIES})
