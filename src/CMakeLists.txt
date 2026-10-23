
add_library(encoding
    huffman_encoding.c
)

target_include_directories(encoding
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

add_library(tree 
    tree.c
)

target_include_directories(tree
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

add_executable(huffman
    main.c
)

target_link_libraries(huffman
    PRIVATE
        encoding
        tree
)
