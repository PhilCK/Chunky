#ifndef INCLUDED_9B209C02_94F8_47AA_9873_C9E05012D83D
#define INCLUDED_9B209C02_94F8_47AA_9873_C9E05012D83D

#define CHUNKY_MAX_COMPONENTS 64

struct chunky_component {
        char name[32];
        size_t bytes;
};

struct chunky_ctx {
        struct chunky_component comps[CHUNKY_MAX_COMPONENTS];
        int comp_count;
};

#endif
