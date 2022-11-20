cmake_minimum_required(VERSION 3.23)
project(labsLinuxOS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS -Wl,--no-as-needed)

find_package (Threads)


add_library(my_dynamic_sentences_library SHARED
            longest_sentence_lib/lab.cpp)
target_include_directories(my_dynamic_sentences_library PUBLIC
                            longest_sentence_lib/include)

add_executable(dynamicRun run_dynamically.cpp)
target_link_libraries(dynamicRun my_dynamic_sentences_library)
target_link_libraries(dynamicRun ${CMAKE_DL_LIBS})
target_link_libraries(dynamicRun dl "-Wl,--no-as-needed" pthread "-Wl,--as-needed")


add_library(my_static_sentences_library STATIC
            longest_sentence_lib/lab.cpp)
target_include_directories(my_static_sentences_library PUBLIC
                            longest_sentence_lib/include)

add_executable(staticRun run_statically.cpp)
target_link_libraries(staticRun my_static_sentences_library)
target_link_libraries(staticRun ${CMAKE_THREAD_LIBS_INIT})

