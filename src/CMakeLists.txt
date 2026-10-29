

add_library(tree 
    tree.c
)

target_include_directories(tree
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)


add_library(encoding
    huffman_encoding.c
)


target_include_directories(encoding
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(encoding
    PUBLIC
        tree
)

add_library(print
    huffman_print.c
)

target_include_directories(print
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(print
    PUBLIC
        encoding
)

add_executable(huffman
    main.c
)

target_link_libraries(huffman
    PRIVATE
        print
        encoding
        tree
)
