cmake_minimum_required(VERSION 3.6)

project(phonotry)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(app "phonotry.cpp" "engine/conf.cpp" "engine/letter.cpp"  "engine/phonotext.cpp" "engine/proccessing.cpp")

target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json)