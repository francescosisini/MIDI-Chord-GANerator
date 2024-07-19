#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smf.h>
#include "chords.h"
#include "melody.h"
#include "key_detection.h"

#define MAX_CHORDS 100
#define MAX_LENGTH 10

void get_chord_sequence(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int *num_chords, int *beats_per_measure, int *num_repeats, int *generate_melody, int *melody_follow_key);
void write_midi(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, int generate_melody, int melody_follow_key, const char* filename);
void get_scale_notes(const char* key, int* scale_notes, int* num_notes);

void get_chord_sequence(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int *num_chords, int *beats_per_measure, int *num_repeats, int *generate_melody, int *melody_follow_key) {
    printf("Scegli il metro (3 per 3/4 o 4 per 4/4): ");
    scanf("%d", beats_per_measure);
    if (*beats_per_measure != 3 && *beats_per_measure != 4) {
        printf("Metro non valido. Impostato a 4/4 per default.\n");
        *beats_per_measure = 4;
    }

    printf("Inserisci il numero di accordi nel giro armonico: ");
    scanf("%d", num_chords);

    printf("Inserisci gli accordi (es. C, G, Am, F) e la loro durata in quarti:\n");
    for (int i = 0; i < *num_chords; i++) {
        printf("Accordo %d: ", i + 1);
        scanf("%s", chords[i]);
        
        int valid_duration = 0;
        while (!valid_duration) {
            printf("Durata in quarti (1, 2, ... %d): ", *beats_per_measure);
            scanf("%d", &durations[i]);
            if (durations[i] >= 1 && durations[i] <= *beats_per_measure) {
                valid_duration = 1;
            } else {
                printf("Durata non valida. Deve essere tra 1 e %d quarti.\n", *beats_per_measure);
            }
        }
    }

    printf("Quante volte ripetere il giro armonico? ");
    scanf("%d", num_repeats);

    printf("Vuoi generare una melodia casuale? (1 per sì, 0 per no): ");
    scanf("%d", generate_melody);

    if (*generate_melody) {
        printf("Vuoi che la melodia segua la tonalità? (1 per sì, 0 per no): ");
        scanf("%d", melody_follow_key);
    } else {
        *melody_follow_key = 0;
    }
}

void write_midi(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, int generate_melody, int melody_follow_key, const char* filename) {
    int notes[4];
    int num_notes;
    int scale_notes[7]; // Scala maggiore diatonica (7 note)
    int num_scale_notes;
    char keys[MAX_CHORDS][MAX_LENGTH];

    // Determina la tonalità per ogni accordo
    determine_keys_for_chords(chords, num_chords, keys);

    smf_t* smf = smf_new();
    smf_track_t* piano_track = smf_track_new();
    smf_track_t* drum_track = smf_track_new();
    smf_track_t* bass_track = smf_track_new();
    smf_track_t* melody_track = smf_track_new();
    smf_add_track(smf, piano_track);
    smf_add_track(smf, drum_track);
    smf_add_track(smf, bass_track);
    smf_add_track(smf, melody_track);

    double time_seconds = 0.0;
    double quarter_note_duration = 0.5; // durata di una nota da un quarto a 120 BPM

    for (int r = 0; r < num_repeats; r++) {
        for (int i = 0; i < num_chords; i++) {
            num_notes = chord_to_notes(chords[i], notes);
            
            // Ottieni la scala della tonalità corrente
            get_scale_notes(keys[i], scale_notes, &num_scale_notes);
            
            // Aggiungi eventi "Note On" per tutte le note dell'accordo nel piano
            for (int j = 0; j < num_notes; j++) {
                smf_event_t* event_on = smf_event_new_from_bytes(0x90, notes[j], 100);
                smf_track_add_event_seconds(piano_track, event_on, time_seconds);
            }

            // Genera note casuali durante l'accordo se richiesto nella traccia melodia
            if (generate_melody) {
                if (melody_follow_key) {
                    generate_random_notes_with_scale(melody_track, 4, time_seconds, quarter_note_duration * durations[i], scale_notes, num_scale_notes);
                } else {
                    generate_random_notes(melody_track, 4, time_seconds, quarter_note_duration * durations[i]);
                }
            }

            // Aggiungi gli eventi per la batteria in base al metro
            for (int k = 0; k < durations[i]; k++) {
                double beat_time = time_seconds + k * quarter_note_duration;

                // Aggiungi la cassa (Kick Drum) sulla prima battuta
                if (k % beats_per_measure == 0) {
                    smf_event_t* kick_on = smf_event_new_from_bytes(0x99, 36, 100);
                    smf_track_add_event_seconds(drum_track, kick_on, beat_time);
                    smf_event_t* kick_off = smf_event_new_from_bytes(0x89, 36, 100);
                    smf_track_add_event_seconds(drum_track, kick_off, beat_time + 0.1);
                }

                // Aggiungi il rullante (Snare Drum) in base al metro
                if ((beats_per_measure == 4 && (k % 4 == 1 || k % 4 == 3)) || (beats_per_measure == 3 && k % 3 == 1)) {
                    smf_event_t* snare_on = smf_event_new_from_bytes(0x99, 38, 100);
                    smf_track_add_event_seconds(drum_track, snare_on, beat_time);
                    smf_event_t* snare_off = smf_event_new_from_bytes(0x89, 38, 100);
                    smf_track_add_event_seconds(drum_track, snare_off, beat_time + 0.1);
                }

                // Aggiungi un hi-hat chiuso (Closed Hi-Hat) su ogni ottavo
                smf_event_t* hihat_on = smf_event_new_from_bytes(0x99, 42, 100);
                smf_track_add_event_seconds(drum_track, hihat_on, beat_time);
                smf_event_t* hihat_off = smf_event_new_from_bytes(0x89, 42, 100);
                smf_track_add_event_seconds(drum_track, hihat_off, beat_time + 0.1);
            }

            // Aggiungi una nota di basso (nota MIDI della fondamentale dell'accordo)
            smf_event_t* bass_on = smf_event_new_from_bytes(0x90, notes[0], 100);
            smf_track_add_event_seconds(bass_track, bass_on, time_seconds);
            smf_event_t* bass_off = smf_event_new_from_bytes(0x80, notes[0], 100);
            smf_track_add_event_seconds(bass_track, bass_off, time_seconds + quarter_note_duration * durations[i]);

            // Incrementa il tempo per la durata dell'accordo
            time_seconds += quarter_note_duration * durations[i];

            // Aggiungi eventi "Note Off" per tutte le note dell'accordo nel piano
            for (int j = 0; j < num_notes; j++) {
                smf_event_t* event_off = smf_event_new_from_bytes(0x80, notes[j], 100);
                smf_track_add_event_seconds(piano_track, event_off, time_seconds);
            }
        }
    }

    if (smf_save(smf, filename) != 0) {
        fprintf(stderr, "Errore durante il salvataggio del file MIDI: %s\n", filename);
    }
    smf_delete(smf);
}

int main() {
    char chords[MAX_CHORDS][MAX_LENGTH];
    int durations[MAX_CHORDS];
    int num_chords;
    int beats_per_measure;
    int num_repeats;
    int generate_melody;
    int melody_follow_key;

    get_chord_sequence(chords, durations, &num_chords, &beats_per_measure, &num_repeats, &generate_melody, &melody_follow_key);
    write_midi(chords, durations, num_chords, beats_per_measure, num_repeats, generate_melody, melody_follow_key, "output.mid");

    printf("File MIDI generato: output.mid\n");
    return 0;
}
