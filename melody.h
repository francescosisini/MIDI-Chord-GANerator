#ifndef MELODY_H
#define MELODY_H

#include <smf.h>

void generate_random_notes(smf_track_t *track, int num_notes, double start_time, double duration);
void generate_random_notes_with_scale(smf_track_t *track, int num_notes, double start_time, double duration, int* scale_notes, int num_scale_notes);
void get_scale_notes(const char* key, int* scale_notes, int* num_notes);

#endif // MELODY_H
