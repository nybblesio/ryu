project(common)

add_library(
        ${PROJECT_NAME}
        bytes.h
        IntervalTree.h
        stream_support.cpp stream_support.h
        string_support.cpp string_support.h)
