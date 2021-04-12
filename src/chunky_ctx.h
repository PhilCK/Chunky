#ifndef INCLUDED_9B209C02_94F8_47AA_9873_C9E05012D83D
#define INCLUDED_9B209C02_94F8_47AA_9873_C9E05012D83D

#include <stdint.h>
#include <stddef.h>

/* -------------------------------------------------------------------------- */
/* Config
 */

#ifndef CHUNKY_MAX_ENTITIES
#define CHUNKY_MAX_ENTITIES 128
#endif

#ifndef CHUNKY_MAX_CHUNKS
#define CHUNKY_MAX_CHUNKS 32
#endif

/* -------------------------------------------------------------------------- */
/* Helpers
 */

#define CHUNKY_MAX_COMPONENTS 64
#define CHUNKY_IS_POW2(i) ((i != 0) && ((i & (i - 1)) == 0))

/* -------------------------------------------------------------------------- */
/* Context Data
 */

struct chunk_info {
        uint64_t layout;
};

struct chunk_block {
        struct chunky_chunk_header header;
        uint8_t data[16384 - sizeof(struct chunky_chunk_header)];
};

struct chunk_layout {
        uint64_t bit_pattern;
        uint16_t deltas[64];
        uint8_t strides[64];
};

struct chunky_entity {
        size_t idx;
        char name[32];
        uint64_t components;
};

struct chunky_component {
        char name[32];
        size_t bytes;
};

struct chunky_ctx {
        struct chunk_info info[CHUNKY_MAX_CHUNKS];
        struct chunk_block block[CHUNKY_MAX_CHUNKS];

        struct chunk_layout layouts[64];
        int layout_count;

        uint8_t entity_state[CHUNKY_MAX_ENTITIES];
        struct chunky_entity entities[CHUNKY_MAX_ENTITIES];

        struct chunky_component comps[CHUNKY_MAX_COMPONENTS];
        int comp_count;
};

/* -------------------------------------------------------------------------- */

#endif
