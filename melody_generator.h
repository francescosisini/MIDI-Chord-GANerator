// melody_generator.h
#ifndef MELODY_GENERATOR_H
#define MELODY_GENERATOR_H

#define MAX_SECTIONS 10
#define MAX_CHORDS 100
#define MAX_LENGTH 10
#define MAX_MELODY_NOTES 128

#define MAX_NOTES 128

#include <stdlib.h>

typedef struct {
    int melody_notes[MAX_CHORDS][MAX_NOTES];
    double melody_durations[MAX_CHORDS][MAX_NOTES];
    int melody_lengths[MAX_CHORDS];
} MelodyResult;

void compose_melody(
    char sections[MAX_SECTIONS][MAX_LENGTH], 
    int section_repeats[MAX_SECTIONS], 
    char section_chords[MAX_SECTIONS][MAX_CHORDS][MAX_LENGTH], 
    int section_durations[MAX_SECTIONS][MAX_CHORDS], 
    int song_structure[MAX_CHORDS][2], 
    int song_structure_length, 
    int beats_per_measure, 
    int instrument, 
    int mood,
    int BPM, 
    MelodyResult* melody_result
);

#endif // MELODY_GENERATOR_H
