mkdir bin

# Utest

clang \
        -I ../include \
        -I ../external/utest.h \
        ../src/lifetime.c \
        ../src/entities.c \
        ../src/components.c \
        ../src/blocks.c \
        ../test/utest.c \
        ../test/utest_lifetime.c \
        ../test/utest_components.c \
        ../test/utest_entities.c \
        ../test/utest_blocks.c \
        -std=c99 -O2 \
        -o \
        bin/chunky_utest

# Ubench

clang \
        -I ../include \
        -I ../external/ubench.h \
        ../src/lifetime.c \
        ../src/entities.c \
        ../src/components.c \
        ../src/blocks.c \
        ../test/ubench.c \
        ../test/ubench_components.c \
        -O2 \
        -o \
        bin/chunky_ubench

