#include "melody_generator.h"

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
) {

  double SEMIMINIMA = 60.0 / BPM;
    double MINIMA = 2 * SEMIMINIMA;
    double CROMA = SEMIMINIMA / 2;
    double SEMICROMA = SEMIMINIMA / 4;
    double SEMIBREVE = 4 * SEMIMINIMA;
  
    for (int i = 0; i < song_structure_length; i++) {
        int section_index = song_structure[i][0];
        int repeat = song_structure[i][1];

        for (int j = 0; j < repeat; j++) {
            for (int k = 0; k < MAX_CHORDS && section_chords[section_index][k][0] != 0; k++) {
                melody_result->melody_lengths[section_index] = 0;
                for (int l = 0; l < 8; l++) {
                    melody_result->melody_notes[section_index][melody_result->melody_lengths[section_index]] = 60 + (rand() % 12);
                    melody_result->melody_durations[section_index][melody_result->melody_lengths[section_index]] = CROMA;
                    melody_result->melody_lengths[section_index]++;
                }
            }
        }
    }
}
