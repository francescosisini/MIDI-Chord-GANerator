// sections.c (implementazione)
#include "section.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static char* xstrdup(const char* s) {
    size_t n = strlen(s) + 1;
    char* p = (char*)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

void sections_init(SectionRegistry* reg) {
    reg->sections = NULL; reg->count = 0; reg->cap = 0;
    reg->song = NULL;     reg->song_count = 0; reg->song_cap = 0;
}

static void free_section(Section* s) {
    if (!s) return;
    free(s->name);
    for (size_t i=0;i<s->count;++i) free(s->items[i].chord);
    free(s->items);
}

void sections_free(SectionRegistry* reg) {
    if (!reg) return;
    for (size_t i=0;i<reg->count;++i) free_section(&reg->sections[i]);
    free(reg->sections);
    for (size_t i=0;i<reg->song_count;++i) free(reg->song[i].name);
    free(reg->song);
}

static int grow_sections(SectionRegistry* reg) {
    if (reg->count < reg->cap) return 1;
    size_t ncap = reg->cap ? reg->cap*2 : 8;
    Section* p = (Section*)realloc(reg->sections, ncap*sizeof(Section));
    if (!p) return 0;
    reg->sections = p; reg->cap = ncap;
    return 1;
}

static int grow_items(Section* s) {
    if (s->count < s->cap) return 1;
    size_t ncap = s->cap ? s->cap*2 : 8;
    ChordDur* p = (ChordDur*)realloc(s->items, ncap*sizeof(ChordDur));
    if (!p) return 0;
    s->items = p; s->cap = ncap;
    return 1;
}

static int grow_song(SectionRegistry* reg) {
    if (reg->song_count < reg->song_cap) return 1;
    size_t ncap = reg->song_cap ? reg->song_cap*2 : 8;
    SongPart* p = (SongPart*)realloc(reg->song, ncap*sizeof(SongPart));
    if (!p) return 0;
    reg->song = p; reg->song_cap = ncap;
    return 1;
}

static int is_section_token(const char* tok) {
    size_t n = strlen(tok);
    return (n >= 2 && tok[0]=='[' && tok[n-1]==']');
}

static void strip_brackets(const char* tok, char* out, size_t outsz) {
    // copia senza [ ]
    size_t n = strlen(tok);
    size_t len = (n>=2)? n-2 : 0;
    if (len >= outsz) len = outsz-1;
    memcpy(out, tok+1, len);
    out[len] = '\0';
}

static int iequals(const char* a, const char* b) {
    for (; *a && *b; ++a, ++b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
    }
    return *a == '\0' && *b == '\0';
}

static Section* find_or_add_section(SectionRegistry* reg, const char* name) {
    for (size_t i=0;i<reg->count;++i) {
        if (iequals(reg->sections[i].name, name)) return &reg->sections[i];
    }
    if (!grow_sections(reg)) return NULL;
    Section* s = &reg->sections[reg->count++];
    s->name = xstrdup(name);
    s->items = NULL; s->count = 0; s->cap = 0;
    return s;
}

const Section* find_section(const SectionRegistry* reg, const char* name) {
    for (size_t i=0;i<reg->count;++i) {
        if (iequals(reg->sections[i].name, name)) return &reg->sections[i];
    }
    return NULL;
}

static int add_item(Section* sec, const char* chord, int duration) {
    if (!grow_items(sec)) return 0;
    sec->items[sec->count].chord = xstrdup(chord);
    sec->items[sec->count].duration = duration;
    sec->count++;
    return 1;
}

static int add_song_part(SectionRegistry* reg, const char* name, int repeats) {
    if (!grow_song(reg)) return 0;
    reg->song[reg->song_count].name = xstrdup(name);
    reg->song[reg->song_count].repeats = repeats;
    reg->song_count++;
    return 1;
}

// Parsing: dopo le opzioni, la CLI ha pattern tipo:
// [INTRO] C 4 G 4 Am 4 F 4 [VERSE] ... [SONG] INTRO 1 VERSE 2 CHORUS 2
int parse_sections_and_structure(SectionRegistry* reg, int argc, char** argv) {
    Section* current = NULL;
    int in_structure = 0;

    for (int i=0; i<argc; ++i) {
        const char* tok = argv[i];

        if (is_section_token(tok)) {
            char name[128];
            strip_brackets(tok, name, sizeof(name));
            if (iequals(name, "SONG")) {
                in_structure = 1;
                current = NULL;
                continue;
            }
            in_structure = 0;
            current = find_or_add_section(reg, name);
            if (!current) { fprintf(stderr, "Out of memory creating section '%s'\n", name); return -2; }
            continue;
        }

        if (!in_structure) {
            // Dentro una sezione: coppie (chord, duration)
            if (!current) {
                fprintf(stderr, "Syntax error: chord '%s' appears outside any [SECTION]\n", tok);
                return -3;
            }
            const char* chord = tok;
            if (i+1 >= argc) {
                fprintf(stderr, "Syntax error: missing duration after chord '%s'\n", chord);
                return -4;
            }
            const char* dur_s = argv[++i];
            char* endp = NULL;
            long d = strtol(dur_s, &endp, 10);
            if (!dur_s[0] || (endp && *endp) || d <= 0) {
                fprintf(stderr, "Invalid duration '%s' after chord '%s'\n", dur_s, chord);
                return -5;
            }
            if (!add_item(current, chord, (int)d)) {
                fprintf(stderr, "Out of memory adding item to section '%s'\n", current->name);
                return -6;
            }
        } else {
            // Struttura [SONG]: coppie (section_name, repeats)
            const char* secname = tok;
            if (i+1 >= argc) {
                fprintf(stderr, "Syntax error: missing repeats after section '%s' in [SONG]\n", secname);
                return -7;
            }
            const char* rep_s = argv[++i];
            char* endp = NULL;
            long r = strtol(rep_s, &endp, 10);
            if (!rep_s[0] || (endp && *endp) || r <= 0) {
                fprintf(stderr, "Invalid repeats '%s' after section '%s'\n", rep_s, secname);
                return -8;
            }
            // (opzionale) validazione: avvisa se la sezione non esiste
            if (!find_section(reg, secname)) {
                fprintf(stderr, "Warning: section '%s' referenced in [SONG] not defined yet.\n", secname);
                // Non blocchiamo: magari l’utente la definisce dopo (ordine libero)
            }
            if (!add_song_part(reg, secname, (int)r)) {
                fprintf(stderr, "Out of memory adding song part '%s'\n", secname);
                return -9;
            }
        }
    }

    // Validazione finale: tutte le sezioni usate in SONG devono esistere
    for (size_t i=0;i<reg->song_count;++i) {
        if (!find_section(reg, reg->song[i].name)) {
            fprintf(stderr, "Error: section '%s' referenced in [SONG] was never defined.\n", reg->song[i].name);
            return -10;
        }
    }

    return 0;
}
