#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <smf.h>
#include "chords.h"
#include "melody.h"
#include "key_detection.h"

#define MAX_CHORDS 100
#define MAX_LENGTH 10

// Globals
double SEMIMINIMA;           // Semiminima (1/4)
double MINIMA;               // Minima (1/2)
double CROMA;                // Croma (1/8)
double SEMICROMA;            // Semicroma (1/16)
double SEMIBREVE;            // Semibreve (intera)



void write_midi_with_melody(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, int include_bass, int include_drums, int generate_melody, int melody_follow_key, int piano_volume, int drum_volume, int bass_volume, int melody_volume, const char* filename);
void get_scale_notes(const char* key, int* scale_notes, int* num_notes);

int main(int argc, char **argv) {
  int BPM = 90;
  int beats_per_measure = 4;
    int num_repeats = 1;
    int include_bass = 0;
    int include_drums = 0;
    int generate_melody = 0;
    int melody_follow_key = 0;
    int piano_volume = 100;
    int drum_volume = 100;
    int bass_volume = 100;
    int melody_volume = 100;
    char chords[MAX_CHORDS][MAX_LENGTH];
    int durations[MAX_CHORDS];
    int num_chords = 0;
    char output_filename[100] = "output.mid";

    int opt;
    while ((opt = getopt(argc, argv, "t:m:r:bdgkp:v:q:w:o:")) != -1) {
        switch (opt) {
            case 'm':
                beats_per_measure = atoi(optarg);
                if (beats_per_measure != 3 && beats_per_measure != 4) {
                    fprintf(stderr, "Invalid meter. Defaulting to 4/4.\n");
                    beats_per_measure = 4;
                }
                break;
            case 'r':
                num_repeats = atoi(optarg);
                break;
            case 'b':
                include_bass = 1;
                break;
            case 'd':
                include_drums = 1;
                break;
            case 'g':
                generate_melody = 1;
                break;
            case 'k':
                melody_follow_key = 1;
                break;
            case 'p':
                piano_volume = atoi(optarg);
                if (piano_volume < 0 || piano_volume > 127) {
                    fprintf(stderr, "Invalid piano volume. Must be between 0 and 127. Defaulting to 100.\n");
                    piano_volume = 100;
                }
                break;
            case 'v':
                drum_volume = atoi(optarg);
                if (drum_volume < 0 || drum_volume > 127) {
                    fprintf(stderr, "Invalid drum volume. Must be between 0 and 127. Defaulting to 100.\n");
                    drum_volume = 100;
                }
                break;
            case 'q':
                bass_volume = atoi(optarg);
                if (bass_volume < 0 || bass_volume > 127) {
                    fprintf(stderr, "Invalid bass volume. Must be between 0 and 127. Defaulting to 100.\n");
                    bass_volume = 100;
                }
                break;
	   case 't':
                BPM = atoi(optarg);
                if (BPM < 1 || BPM > 200) {
                    fprintf(stderr, "Invalid BPM. Must be between 1 and 200. Defaulting to 90.\n");
                }
                break;
            case 'w':
                melody_volume = atoi(optarg);
                if (melody_volume < 0 || melody_volume > 127) {
                    fprintf(stderr, "Invalid melody volume. Must be between 0 and 127. Defaulting to 100.\n");
                    melody_volume = 100;
                }
                break;
            case 'o':
                strcpy(output_filename, optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-m meter] [-r repeats] [-b] [-d] [-g] [-k] [-p piano_volume] [-v drum_volume] [-q bass_volume] [-w melody_volume] [-o output_filename] chord1 duration1 chord2 duration2 ...\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Eval notes duration
    // Durate delle note in secondi, basate sui BPM definiti
    SEMIMINIMA = 60.0 / BPM;           // Semiminima (1/4)
    MINIMA = 2 * SEMIMINIMA;           // Minima (1/2)
    CROMA = SEMIMINIMA / 2;            // Croma (1/8)
    SEMICROMA = SEMIMINIMA / 4;        // Semicroma (1/16)
    SEMIBREVE = 4 * SEMIMINIMA;        // Semibreve (intera)

    
    // Parse chords and durations
    int remaining_args = argc - optind;
    if (remaining_args % 2 != 0) {
        fprintf(stderr, "Each chord must have a corresponding duration.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = optind; i < argc; i += 2) {
        if (i + 1 >= argc) {
            fprintf(stderr, "Missing duration for chord %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
        strcpy(chords[num_chords], argv[i]);
        durations[num_chords] = atoi(argv[i + 1]);
        num_chords++;
    }

    if (num_chords == 0) {
        fprintf(stderr, "No chords provided.\n");
        exit(EXIT_FAILURE);
    }

    write_midi_with_melody(chords, durations, num_chords, beats_per_measure, num_repeats, include_bass, include_drums, generate_melody, melody_follow_key, piano_volume, drum_volume, bass_volume, melody_volume, output_filename);

    printf("File MIDI generato: %s\n", output_filename);
    return 0;
}

void write_midi_with_melody(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, int include_bass, int include_drums, int generate_melody, int melody_follow_key, int piano_volume, int drum_volume, int bass_volume, int melody_volume, const char* filename) {
    smf_t* smf = smf_new();
    smf_track_t* piano_track = smf_track_new();
    smf_add_track(smf, piano_track);
    
    smf_track_t* melody_track = smf_track_new();
    smf_add_track(smf, melody_track);
    
    smf_track_t* bass_track = smf_track_new();
    smf_add_track(smf, bass_track);
    
    smf_track_t* drum_track = smf_track_new();
    smf_add_track(smf, drum_track);

    double time_seconds = 0.0;
    int scale_notes[128];
    int num_scale_notes = 7;

    for (int r = 0; r < num_repeats; r++) {
        for (int i = 0; i < num_chords; i++) {
            int notes[10];
            int num_notes = chord_to_notes(chords[i], notes);

            // Aggiungi note dell'accordo al piano
            for (int j = 0; j < num_notes; j++) {
                smf_event_t* note_on = smf_event_new_from_bytes(0x90|0x01, notes[j], piano_volume);
                smf_track_add_event_seconds(piano_track, note_on, time_seconds);
                smf_event_t* note_off = smf_event_new_from_bytes(0x80|0x01, notes[j], piano_volume);
                smf_track_add_event_seconds(piano_track, note_off, time_seconds + durations[i] * SEMIMINIMA);
            }

            // Aggiungi melodia se richiesto
            if (generate_melody) {
                if (melody_follow_key) {
                    get_scale_notes(chords[i], scale_notes, &num_scale_notes);
                } else {
                    // Usa tutte le note possibili se non si segue la tonalità
                    for (int k = 0; k < 128; k++) {
                        scale_notes[k] = k;
                    }
                    num_scale_notes = 128;
                }
                generate_varied_duration_melody(melody_track, num_notes, time_seconds, durations[i] * SEMIMINIMA, scale_notes, num_scale_notes, melody_volume);
            }

            // Aggiungi basso se richiesto
            if (include_bass) {
                int bass_note = notes[0] - 12; // Un'ottava sotto la tonica
                smf_event_t* bass_on = smf_event_new_from_bytes(0x90|0x02, bass_note, bass_volume);
                smf_track_add_event_seconds(bass_track, bass_on, time_seconds);
                smf_event_t* bass_off = smf_event_new_from_bytes(0x80|0x02, bass_note, bass_volume);
                smf_track_add_event_seconds(bass_track, bass_off, time_seconds + durations[i] * SEMIMINIMA);
            }

            // Aggiungi batteria se richiesta
            if (include_drums) {
                // Aggiungi cassa (Kick drum)
                int kick_drum_note = 36; // Cassa
                smf_event_t* kick_drum_on = smf_event_new_from_bytes(0x99, kick_drum_note, drum_volume); // Canale 10 per batteria
                smf_track_add_event_seconds(drum_track, kick_drum_on, time_seconds);
                smf_event_t* kick_drum_off = smf_event_new_from_bytes(0x89, kick_drum_note, drum_volume); // Canale 10 per batteria
                smf_track_add_event_seconds(drum_track, kick_drum_off, time_seconds + SEMIMINIMA); // Durata 1/4

                // Aggiungi rullante (Snare drum)
                int snare_drum_note = 38; // Rullante
                smf_event_t* snare_drum_on = smf_event_new_from_bytes(0x99, snare_drum_note, drum_volume); // Canale 10 per batteria
                smf_track_add_event_seconds(drum_track, snare_drum_on, time_seconds + MINIMA); // Aggiungi rullante a metà battuta
                smf_event_t* snare_drum_off = smf_event_new_from_bytes(0x89, snare_drum_note, drum_volume); // Canale 10 per batteria
                smf_track_add_event_seconds(drum_track, snare_drum_off, time_seconds + MINIMA+SEMIMINIMA); // Durata 1/4

                // Aggiungi hi-hat chiuso
                int hi_hat_note = 42; // Hi-hat chiuso
                for (int beat = 0; beat < beats_per_measure; beat++) {
                    smf_event_t* hi_hat_on = smf_event_new_from_bytes(0x99, hi_hat_note, drum_volume); // Canale 10 per batteria
                    smf_track_add_event_seconds(drum_track, hi_hat_on, time_seconds + beat * SEMIMINIMA);
                    smf_event_t* hi_hat_off = smf_event_new_from_bytes(0x89, hi_hat_note, drum_volume); // Canale 10 per batteria
                    smf_track_add_event_seconds(drum_track, hi_hat_off, time_seconds + beat * SEMIMINIMA + CROMA); // Durata 1/8
                }
            }

            time_seconds += durations[i] * SEMIMINIMA;
        }
    }

    int result = smf_save(smf, filename); // Salva il file MIDI e controlla il risultato
    if (result != 0) {
        fprintf(stderr, "Errore nel salvataggio del file MIDI: %s\n", filename);
    }
    
    smf_delete(smf);
}

