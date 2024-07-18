#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smf.h>


#define MAX_CHORDS 100
#define MAX_LENGTH 10

void get_chord_sequence(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int *num_chords, int *beats_per_measure, int *num_repeats);
int chord_to_notes(const char* chord, int* notes);
void write_midi(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, const char* filename);

void get_chord_sequence(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int *num_chords, int *beats_per_measure, int *num_repeats) {
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
}

void write_midi(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, const char* filename) {
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

    get_chord_sequence(chords, durations, &num_chords, &beats_per_measure, &num_repeats);
    write_midi(chords, durations, num_chords, beats_per_measure, num_repeats, "output.mid");

    printf("File MIDI generato: output.mid\n");
    return 0;
}



// Mappa degli accordi alle loro note MIDI corrispondenti
int chord_to_notes(const char* chord, int* notes) {
    // Accordi maggiori
    if (strcmp(chord, "C") == 0) {
        notes[0] = 60; notes[1] = 64; notes[2] = 67; // C E G
        return 3;
    } else if (strcmp(chord, "C#") == 0 || strcmp(chord, "Db") == 0) {
        notes[0] = 61; notes[1] = 65; notes[2] = 68; // C# F G#
        return 3;
    } else if (strcmp(chord, "D") == 0) {
        notes[0] = 62; notes[1] = 66; notes[2] = 69; // D F# A
        return 3;
    } else if (strcmp(chord, "D#") == 0 || strcmp(chord, "Eb") == 0) {
        notes[0] = 63; notes[1] = 67; notes[2] = 70; // D# G A#
        return 3;
    } else if (strcmp(chord, "E") == 0) {
        notes[0] = 64; notes[1] = 68; notes[2] = 71; // E G# B
        return 3;
    } else if (strcmp(chord, "F") == 0) {
        notes[0] = 65; notes[1] = 69; notes[2] = 72; // F A C
        return 3;
    } else if (strcmp(chord, "F#") == 0 || strcmp(chord, "Gb") == 0) {
        notes[0] = 66; notes[1] = 70; notes[2] = 73; // F# A# C#
        return 3;
    } else if (strcmp(chord, "G") == 0) {
        notes[0] = 67; notes[1] = 71; notes[2] = 74; // G B D
        return 3;
    } else if (strcmp(chord, "G#") == 0 || strcmp(chord, "Ab") == 0) {
        notes[0] = 68; notes[1] = 72; notes[2] = 75; // G# C D#
        return 3;
    } else if (strcmp(chord, "A") == 0) {
        notes[0] = 69; notes[1] = 73; notes[2] = 76; // A C# E
        return 3;
    } else if (strcmp(chord, "A#") == 0 || strcmp(chord, "Bb") == 0) {
        notes[0] = 70; notes[1] = 74; notes[2] = 77; // A# D F
        return 3;
    } else if (strcmp(chord, "B") == 0) {
        notes[0] = 71; notes[1] = 75; notes[2] = 79; // B D# F#
        return 3;
    }
    // Accordi minori
    else if (strcmp(chord, "Cm") == 0) {
        notes[0] = 60; notes[1] = 63; notes[2] = 67; // C D# G
        return 3;
    } else if (strcmp(chord, "C#m") == 0 || strcmp(chord, "Dbm") == 0) {
        notes[0] = 61; notes[1] = 64; notes[2] = 68; // C# E G#
        return 3;
    } else if (strcmp(chord, "Dm") == 0) {
        notes[0] = 62; notes[1] = 65; notes[2] = 69; // D F A
        return 3;
    } else if (strcmp(chord, "D#m") == 0 || strcmp(chord, "Ebm") == 0) {
        notes[0] = 63; notes[1] = 66; notes[2] = 70; // D# F# A#
        return 3;
    } else if (strcmp(chord, "Em") == 0) {
        notes[0] = 64; notes[1] = 67; notes[2] = 71; // E G B
        return 3;
    } else if (strcmp(chord, "Fm") == 0) {
        notes[0] = 65; notes[1] = 68; notes[2] = 72; // F G# C
        return 3;
    } else if (strcmp(chord, "F#m") == 0 || strcmp(chord, "Gbm") == 0) {
        notes[0] = 66; notes[1] = 69; notes[2] = 73; // F# A C#
        return 3;
    } else if (strcmp(chord, "Gm") == 0) {
        notes[0] = 67; notes[1] = 70; notes[2] = 74; // G A# D
        return 3;
    } else if (strcmp(chord, "G#m") == 0 || strcmp(chord, "Abm") == 0) {
        notes[0] = 68; notes[1] = 71; notes[2] = 75; // G# B D#
        return 3;
    } else if (strcmp(chord, "Am") == 0) {
        notes[0] = 69; notes[1] = 72; notes[2] = 76; // A C E
        return 3;
    } else if (strcmp(chord, "A#m") == 0 || strcmp(chord, "Bbm") == 0) {
        notes[0] = 70; notes[1] = 73; notes[2] = 77; // A# C# F
        return 3;
    } else if (strcmp(chord, "Bm") == 0) {
        notes[0] = 71; notes[1] = 74; notes[2] = 79; // B D F#
        return 3;
    }
    // Accordi di settima maggiore
    else if (strcmp(chord, "Cmaj7") == 0) {
        notes[0] = 60; notes[1] = 64; notes[2] = 67; notes[3] = 71; // C E G B
        return 4;
    } else if (strcmp(chord, "C#maj7") == 0 || strcmp(chord, "Dbmaj7") == 0) {
        notes[0] = 61; notes[1] = 65; notes[2] = 68; notes[3] = 72; // C# F G# C
        return 4;
    } else if (strcmp(chord, "Dmaj7") == 0) {
        notes[0] = 62; notes[1] = 66; notes[2] = 69; notes[3] = 73; // D F# A C#
        return 4;
    } else if (strcmp(chord, "D#maj7") == 0 || strcmp(chord, "Ebmaj7") == 0) {
        notes[0] = 63; notes[1] = 67; notes[2] = 70; notes[3] = 74; // D# G A# D
        return 4;
    } else if (strcmp(chord, "Emaj7") == 0) {
        notes[0] = 64; notes[1] = 68; notes[2] = 71; notes[3] = 75; // E G# B D#
        return 4;
    } else if (strcmp(chord, "Fmaj7") == 0) {
        notes[0] = 65; notes[1] = 69; notes[2] = 72; notes[3] = 76; // F A C E
        return 4;
    } else if (strcmp(chord, "F#maj7") == 0 || strcmp(chord, "Gbmaj7") == 0) {
        notes[0] = 66; notes[1] = 70; notes[2] = 73; notes[3] = 77; // F# A# C# F
        return 4;
    } else if (strcmp(chord, "Gmaj7") == 0) {
        notes[0] = 67; notes[1] = 71; notes[2] = 74; notes[3] = 78; // G B D F#
        return 4;
    } else if (strcmp(chord, "G#maj7") == 0 || strcmp(chord, "Abmaj7") == 0) {
        notes[0] = 68; notes[1] = 72; notes[2] = 75; notes[3] = 79; // G# C D# G
        return 4;
    } else if (strcmp(chord, "Amaj7") == 0) {
        notes[0] = 69; notes[1] = 73; notes[2] = 76; notes[3] = 80; // A C# E G#
        return 4;
    } else if (strcmp(chord, "A#maj7") == 0 || strcmp(chord, "Bbmaj7") == 0) {
        notes[0] = 70; notes[1] = 74; notes[2] = 77; notes[3] = 81; // A# D F A
        return 4;
    } else if (strcmp(chord, "Bmaj7") == 0) {
        notes[0] = 71; notes[1] = 75; notes[2] = 79; notes[3] = 83; // B D# F# A#
        return 4;
    }
    // Accordi di settima naturale
    else if (strcmp(chord, "C7") == 0) {
        notes[0] = 60; notes[1] = 64; notes[2] = 67; notes[3] = 70; // C E G A#
        return 4;
    } else if (strcmp(chord, "C#7") == 0 || strcmp(chord, "Db7") == 0) {
        notes[0] = 61; notes[1] = 65; notes[2] = 68; notes[3] = 71; // C# F G# B
        return 4;
    } else if (strcmp(chord, "D7") == 0) {
        notes[0] = 62; notes[1] = 66; notes[2] = 69; notes[3] = 72; // D F# A C
        return 4;
    } else if (strcmp(chord, "D#7") == 0 || strcmp(chord, "Eb7") == 0) {
        notes[0] = 63; notes[1] = 67; notes[2] = 70; notes[3] = 73; // D# G A# C#
        return 4;
    } else if (strcmp(chord, "E7") == 0) {
        notes[0] = 64; notes[1] = 68; notes[2] = 71; notes[3] = 74; // E G# B D
        return 4;
    } else if (strcmp(chord, "F7") == 0) {
        notes[0] = 65; notes[1] = 69; notes[2] = 72; notes[3] = 75; // F A C D#
        return 4;
    } else if (strcmp(chord, "F#7") == 0 || strcmp(chord, "Gb7") == 0) {
        notes[0] = 66; notes[1] = 70; notes[2] = 73; notes[3] = 76; // F# A# C# E
        return 4;
    } else if (strcmp(chord, "G7") == 0) {
        notes[0] = 67; notes[1] = 71; notes[2] = 74; notes[3] = 77; // G B D F
        return 4;
    } else if (strcmp(chord, "G#7") == 0 || strcmp(chord, "Ab7") == 0) {
        notes[0] = 68; notes[1] = 72; notes[2] = 75; notes[3] = 78; // G# C D# F#
        return 4;
    } else if (strcmp(chord, "A7") == 0) {
        notes[0] = 69; notes[1] = 73; notes[2] = 76; notes[3] = 79; // A C# E G
        return 4;
    } else if (strcmp(chord, "A#7") == 0 || strcmp(chord, "Bb7") == 0) {
        notes[0] = 70; notes[1] = 74; notes[2] = 77; notes[3] = 80; // A# D F G#
        return 4;
    } else if (strcmp(chord, "B7") == 0) {
        notes[0] = 71; notes[1] = 75; notes[2] = 79; notes[3] = 82; // B D# F# A
        return 4;
    }

    return 0; // Accord non riconosciuto
}

