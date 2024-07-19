#include "melody.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Mappa per le scale maggiori diatoniche
const int major_scale_intervals[] = {2, 2, 1, 2, 2, 2, 1};

void generate_random_notes(smf_track_t* track, int num_notes, double start_time, double duration, int volume) {
    srand(time(NULL));
    double note_duration = duration / num_notes;
    for (int i = 0; i < num_notes; i++) {
        int note = rand() % 128; // Note casuale tra 0 e 127
        smf_event_t* note_on = smf_event_new_from_bytes(0x90, note, volume);
        smf_track_add_event_seconds(track, note_on, start_time + i * note_duration);
        smf_event_t* note_off = smf_event_new_from_bytes(0x80, note, volume);
        smf_track_add_event_seconds(track, note_off, start_time + (i + 1) * note_duration);
    }
}

void generate_random_notes_with_scale(smf_track_t* track, int num_notes, double start_time, double duration, int* scale_notes, int num_scale_notes, int volume) {
    srand(time(NULL));
    double note_duration = duration / num_notes;
    for (int i = 0; i < num_notes; i++) {
        int note = scale_notes[rand() % num_scale_notes]; // Note casuale dalla scala
        smf_event_t* note_on = smf_event_new_from_bytes(0x90, note, volume);
        smf_track_add_event_seconds(track, note_on, start_time + i * note_duration);
        smf_event_t* note_off = smf_event_new_from_bytes(0x80, note, volume);
        smf_track_add_event_seconds(track, note_off, start_time + (i + 1) * note_duration);
    }
}

void get_scale_notes(const char* key, int* scale_notes, int* num_notes) {
    int base_note;
    *num_notes = 7; // Una scala diatonica maggiore ha 7 note

    // Determina la nota base dalla tonalità
    if (strcmp(key, "C") == 0) {
        base_note = 60; // C
    } else if (strcmp(key, "C#") == 0 || strcmp(key, "Db") == 0) {
        base_note = 61; // C# / Db
    } else if (strcmp(key, "D") == 0) {
        base_note = 62; // D
    } else if (strcmp(key, "D#") == 0 || strcmp(key, "Eb") == 0) {
        base_note = 63; // D# / Eb
    } else if (strcmp(key, "E") == 0) {
        base_note = 64; // E
    } else if (strcmp(key, "F") == 0) {
        base_note = 65; // F
    } else if (strcmp(key, "F#") == 0 || strcmp(key, "Gb") == 0) {
        base_note = 66; // F# / Gb
    } else if (strcmp(key, "G") == 0) {
        base_note = 67; // G
    } else if (strcmp(key, "G#") == 0 || strcmp(key, "Ab") == 0) {
        base_note = 68; // G# / Ab
    } else if (strcmp(key, "A") == 0) {
        base_note = 69; // A
    } else if (strcmp(key, "A#") == 0 || strcmp(key, "Bb") == 0) {
        base_note = 70; // A# / Bb
    } else if (strcmp(key, "B") == 0) {
        base_note = 71; // B
    } else {
        base_note = 60; // Default to C
    }

    // Costruisci la scala maggiore diatonica basata sulla nota base
    scale_notes[0] = base_note;
    for (int i = 1; i < *num_notes; i++) {
        scale_notes[i] = scale_notes[i - 1] + major_scale_intervals[i - 1];
    }
}
