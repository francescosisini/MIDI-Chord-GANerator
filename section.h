// sections.h (interfaccia semplice)
#pragma once
#include <stddef.h>

typedef struct {
    char* chord;   // es. "C", "Am", "Gadd9"
    int   duration; // in battiti
} ChordDur;

typedef struct {
    char* name;       // es. "INTRO", "Verse_A"
    ChordDur* items;  // lista (dinamica)
    size_t count;
    size_t cap;
} Section;

typedef struct {
    char* name;  // sezione da ripetere
    int   repeats;
} SongPart;

typedef struct {
    Section* sections; // registro
    size_t   count;
    size_t   cap;

    SongPart* song;    // struttura [SONG]
    size_t    song_count;
    size_t    song_cap;
} SectionRegistry;

// API
void sections_init(SectionRegistry* reg);
void sections_free(SectionRegistry* reg);

// Parsing dagli argv (dopo le options): ritorna 0 se ok, <0 se errore
int parse_sections_and_structure(SectionRegistry* reg, int argc, char** argv);

// Utility: cerca sezione per nome (NULL se non trovata)
const Section* find_section(const SectionRegistry* reg, const char* name);
