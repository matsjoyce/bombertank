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
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    message.proto
    create_msg.proto
    generic_msg.proto
)

add_executable(main ${SOURCE} main.cpp ${PROTO_SRCS})
target_link_libraries(main sfml-graphics sfml-audio ${Protobuf_LIBRARIES})
