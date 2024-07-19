#ifndef KEY_DETECTION_H
#define KEY_DETECTION_H

#define MAX_CHORDS 100
#define MAX_LENGTH 10

char* determine_key(char chords[MAX_CHORDS][MAX_LENGTH], int num_chords);
void determine_keys_for_chords(char chords[MAX_CHORDS][MAX_LENGTH], int num_chords, char keys[MAX_CHORDS][MAX_LENGTH]);

#endif // KEY_DETECTION_H
