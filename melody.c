#include "melody.h"
#include <stdlib.h>
#include <string.h>

// Funzione per ottenere le note della scala maggiore data una tonalità
void get_scale_notes(const char* key, int* scale_notes, int* num_notes) {
    if (strcmp(key, "C") == 0) {
        int c_major[] = {60, 62, 64, 65, 67, 69, 71}; // C D E F G A B
        memcpy(scale_notes, c_major, sizeof(c_major));
    } else if (strcmp(key, "C#") == 0 || strcmp(key, "Db") == 0) {
        int csharp_major[] = {61, 63, 65, 66, 68, 70, 72}; // C# D# F F# G# A# C
        memcpy(scale_notes, csharp_major, sizeof(csharp_major));
    } else if (strcmp(key, "D") == 0) {
        int d_major[] = {62, 64, 66, 67, 69, 71, 73}; // D E F# G A B C#
        memcpy(scale_notes, d_major, sizeof(d_major));
    } else if (strcmp(key, "D#") == 0 || strcmp(key, "Eb") == 0) {
        int dsharp_major[] = {63, 65, 67, 68, 70, 72, 74}; // D# F G G# A# C D
        memcpy(scale_notes, dsharp_major, sizeof(dsharp_major));
    } else if (strcmp(key, "E") == 0) {
        int e_major[] = {64, 66, 68, 69, 71, 73, 75}; // E F# G# A B C# D#
        memcpy(scale_notes, e_major, sizeof(e_major));
    } else if (strcmp(key, "F") == 0) {
        int f_major[] = {65, 67, 69, 70, 72, 74, 76}; // F G A Bb C D E
        memcpy(scale_notes, f_major, sizeof(f_major));
    } else if (strcmp(key, "F#") == 0 || strcmp(key, "Gb") == 0) {
        int fsharp_major[] = {66, 68, 70, 71, 73, 75, 77}; // F# G# A# B C# D# F
        memcpy(scale_notes, fsharp_major, sizeof(fsharp_major));
    } else if (strcmp(key, "G") == 0) {
        int g_major[] = {67, 69, 71, 72, 74, 76, 78}; // G A B C D E F#
        memcpy(scale_notes, g_major, sizeof(g_major));
    } else if (strcmp(key, "G#") == 0 || strcmp(key, "Ab") == 0) {
        int gsharp_major[] = {68, 70, 72, 73, 75, 77, 79}; // G# A# C C# D# F G
        memcpy(scale_notes, gsharp_major, sizeof(gsharp_major));
    } else if (strcmp(key, "A") == 0) {
        int a_major[] = {69, 71, 73, 74, 76, 78, 80}; // A B C# D E F# G#
        memcpy(scale_notes, a_major, sizeof(a_major));
    } else if (strcmp(key, "A#") == 0 || strcmp(key, "Bb") == 0) {
        int asharp_major[] = {70, 72, 74, 75, 77, 79, 81}; // A# C D D# F G A
        memcpy(scale_notes, asharp_major, sizeof(asharp_major));
    } else if (strcmp(key, "B") == 0) {
        int b_major[] = {71, 73, 75, 76, 78, 80, 82}; // B C# D# E F# G# A#
        memcpy(scale_notes, b_major, sizeof(b_major));
    } else {
        // Se la tonalità non è riconosciuta, di default usa la scala di C maggiore
        int c_major[] = {60, 62, 64, 65, 67, 69, 71}; // C D E F G A B
        memcpy(scale_notes, c_major, sizeof(c_major));
    }
    *num_notes = 7; // Ogni scala maggiore ha 7 note
}

void generate_random_notes(smf_track_t *track, int num_notes, double start_time, double duration) {
    for (int i = 0; i < num_notes; i++) {
        int note = 60 + rand() % 25; // Genera una nota casuale tra C4 e C6
        double note_start = start_time + ((double)rand() / RAND_MAX) * duration;
        double note_duration = ((double)rand() / RAND_MAX) * duration / 2;

        // Aggiungi evento "Note On"
        smf_event_t* event_on = smf_event_new_from_bytes(0x90, note, 100);
        smf_track_add_event_seconds(track, event_on, note_start);

        // Aggiungi evento "Note Off"
        smf_event_t* event_off = smf_event_new_from_bytes(0x80, note, 100);
        smf_track_add_event_seconds(track, event_off, note_start + note_duration);
    }
}

void generate_random_notes_with_scale(smf_track_t *track, int num_notes, double start_time, double duration, int* scale_notes, int num_scale_notes) {
    for (int i = 0; i < num_notes; i++) {
        int note = scale_notes[rand() % num_scale_notes]; // Genera una nota casuale dalla scala
        double note_start = start_time + ((double)rand() / RAND_MAX) * duration;
        double note_duration = ((double)rand() / RAND_MAX) * duration / 2;

        // Aggiungi evento "Note On"
        smf_event_t* event_on = smf_event_new_from_bytes(0x90, note, 100);
        smf_track_add_event_seconds(track, event_on, note_start);

        // Aggiungi evento "Note Off"
        smf_event_t* event_off = smf_event_new_from_bytes(0x80, note, 100);
        smf_track_add_event_seconds(track, event_off, note_start + note_duration);
    }
}
