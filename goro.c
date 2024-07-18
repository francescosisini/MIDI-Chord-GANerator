#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smf.h>
#include "chords.h"
#include "melody.h"

#define MAX_CHORDS 100
#define MAX_LENGTH 10

void get_chord_sequence(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int *num_chords, int *beats_per_measure, int *num_repeats, int *generate_melody);
void write_midi(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, int generate_melody, const char* filename);

void get_chord_sequence(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int *num_chords, int *beats_per_measure, int *num_repeats, int *generate_melody) {
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
}

void write_midi(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, int generate_melody, const char* filename) {
    int notes[4];
    int num_notes;

    smf_t* smf = smf_new();
    smf_track_t* track = smf_track_new();
    smf_add_track(smf, track);

    double time_seconds = 0.0;
    double quarter_note_duration = 0.5; // durata di una nota da un quarto a 120 BPM

    for (int r = 0; r < num_repeats; r++) {
        for (int i = 0; i < num_chords; i++) {
            num_notes = chord_to_notes(chords[i], notes);
            
            // Aggiungi eventi "Note On" per tutte le note dell'accordo
            for (int j = 0; j < num_notes; j++) {
                smf_event_t* event_on = smf_event_new_from_bytes(0x90, notes[j], 100);
                smf_track_add_event_seconds(track, event_on, time_seconds);
            }

            // Genera note casuali durante l'accordo se richiesto
            if (generate_melody) {
                generate_random_notes(track, 4, time_seconds, quarter_note_duration * durations[i]);
            }

            // Incrementa il tempo per la durata dell'accordo
            time_seconds += quarter_note_duration * durations[i];

            // Aggiungi eventi "Note Off" per tutte le note dell'accordo
            for (int j = 0; j < num_notes; j++) {
                smf_event_t* event_off = smf_event_new_from_bytes(0x80, notes[j], 100);
                smf_track_add_event_seconds(track, event_off, time_seconds);
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

    get_chord_sequence(chords, durations, &num_chords, &beats_per_measure, &num_repeats, &generate_melody);
    write_midi(chords, durations, num_chords, beats_per_measure, num_repeats, generate_melody, "output.mid");

    printf("File MIDI generato: output.mid\n");
    return 0;
}
