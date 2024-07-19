#include "melody.h"
#include <stdlib.h>

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
