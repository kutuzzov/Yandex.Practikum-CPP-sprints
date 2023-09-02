cmake_minimum_required(VERSION 3.10)

project(PhoneBook CXX)
set(CMAKE_CXX_STANDARD 17)

set(Protobuf_PREFIX_PATH
    "/opt/homebrew/Cellar/protobuf@21/21.12/include"            
    "/opt/homebrew/Cellar/protobuf@21/21.12/lib"             
    "/opt/homebrew/Cellar/protobuf@21/21.12/bin" )
list(APPEND CMAKE_PREFIX_PATH "${Protobuf_PREFIX_PATH}")

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS contact.proto)

set(PHONEBOOK_FILES iterator_range.h main.cpp phone_book.h phone_book.cpp contact.proto)

add_executable(phonebook ${PROTO_SRCS} ${PROTO_HDRS} ${PHONEBOOK_FILES})
target_include_directories(phonebook PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(phonebook PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(phonebook ${Protobuf_LIBRARY} Threads::Threads) 