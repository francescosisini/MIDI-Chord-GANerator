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

void write_midi_with_melody(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, int include_bass, int include_drums, int generate_melody, int melody_follow_key, int piano_volume, int drum_volume, int bass_volume, int melody_volume, const char* filename);
void get_scale_notes(const char* key, int* scale_notes, int* num_notes);

int main(int argc, char **argv) {
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
    while ((opt = getopt(argc, argv, "m:r:bdgkp:v:q:w:o:")) != -1) {
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

    double time_seconds = 0.0;
    int scale_notes[128];
    int num_scale_notes = 7;

    for (int r = 0; r < num_repeats; r++) {
        for (int i = 0; i < num_chords; i++) {
            int notes[10];
            int num_notes = chord_to_notes(chords[i], notes);

            // Aggiungi note dell'accordo
            for (int j = 0; j < num_notes; j++) {
                smf_event_t* note_on = smf_event_new_from_bytes(0x90, notes[j], piano_volume);
                smf_track_add_event_seconds(piano_track, note_on, time_seconds);
                smf_event_t* note_off = smf_event_new_from_bytes(0x80, notes[j], piano_volume);
                smf_track_add_event_seconds(piano_track, note_off, time_seconds + durations[i] * 0.25);
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
                generate_varied_duration_melody(melody_track, num_notes, time_seconds, durations[i] * 0.25, scale_notes, num_scale_notes, melody_volume);
            }

            time_seconds += durations[i] * 0.25;
        }
    }

    smf_save(smf, filename);
    smf_delete(smf);
}


void _write_midi_with_melody(char chords[MAX_CHORDS][MAX_LENGTH], int *durations, int num_chords, int beats_per_measure, int num_repeats, int include_bass, int include_drums, int generate_melody, int melody_follow_key, int piano_volume, int drum_volume, int bass_volume, int melody_volume, const char* filename) {
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
    if (include_drums) smf_add_track(smf, drum_track);
    if (include_bass) smf_add_track(smf, bass_track);
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
                smf_event_t* event_on = smf_event_new_from_bytes(0x90, notes[j], piano_volume);
                smf_track_add_event_seconds(piano_track, event_on, time_seconds);
            }

            // Genera note casuali durante l'accordo se richiesto nella traccia melodia
            if (generate_melody) {
                if (melody_follow_key) {

		  generate_varied_duration_melody(melody_track, 4, time_seconds, quarter_note_duration * durations[i], scale_notes, num_scale_notes, melody_volume);
		  //generate_melody_with_rules(melody_track, 4, time_seconds, quarter_note_duration * durations[i], scale_notes, num_scale_notes, melody_volume);
                    //generate_random_notes_with_scale(melody_track, 4, time_seconds, quarter_note_duration * durations[i], scale_notes, num_scale_notes, melody_volume);
		    
                } else {
                    generate_random_notes(melody_track, 4, time_seconds, quarter_note_duration * durations[i], melody_volume);
                }
            }

            // Aggiungi gli eventi per la batteria in base al metro, se richiesto
            if (include_drums) {
                for (int k = 0; k < durations[i]; k++) {
                    double beat_time = time_seconds + k * quarter_note_duration;

                    // Aggiungi la cassa (Kick Drum) sulla prima battuta
                    if (k % beats_per_measure == 0) {
                        smf_event_t* kick_on = smf_event_new_from_bytes(0x99, 36, drum_volume);
                        smf_track_add_event_seconds(drum_track, kick_on, beat_time);
                        smf_event_t* kick_off = smf_event_new_from_bytes(0x89, 36, drum_volume);
                        smf_track_add_event_seconds(drum_track, kick_off, beat_time + 0.1);
                    }

                    // Aggiungi il rullante (Snare Drum) in base al metro
                    if ((beats_per_measure == 4 && (k % 4 == 1 || k % 4 == 3)) || (beats_per_measure == 3 && k % 3 == 1)) {
                        smf_event_t* snare_on = smf_event_new_from_bytes(0x99, 38, drum_volume);
                        smf_track_add_event_seconds(drum_track, snare_on, beat_time);
                        smf_event_t* snare_off = smf_event_new_from_bytes(0x89, 38, drum_volume);
                        smf_track_add_event_seconds(drum_track, snare_off, beat_time + 0.1);
                    }

                    // Aggiungi un hi-hat chiuso (Closed Hi-Hat) su ogni ottavo
                    smf_event_t* hihat_on = smf_event_new_from_bytes(0x99, 42, drum_volume);
                    smf_track_add_event_seconds(drum_track, hihat_on, beat_time);
                    smf_event_t* hihat_off = smf_event_new_from_bytes(0x89, 42, drum_volume);
                    smf_track_add_event_seconds(drum_track, hihat_off, beat_time + 0.1);
                }
            }

            // Aggiungi una nota di basso (nota MIDI della fondamentale dell'accordo), se richiesto
            if (include_bass) {
                smf_event_t* bass_on = smf_event_new_from_bytes(0x90, notes[0], bass_volume);
                smf_track_add_event_seconds(bass_track, bass_on, time_seconds);
                smf_event_t* bass_off = smf_event_new_from_bytes(0x80, notes[0], bass_volume);
                smf_track_add_event_seconds(bass_track, bass_off, time_seconds + quarter_note_duration * durations[i]);
            }

            // Incrementa il tempo per la durata dell'accordo
            time_seconds += quarter_note_duration * durations[i];

            // Aggiungi eventi "Note Off" per tutte le note dell'accordo nel piano
            for (int j = 0; j < num_notes; j++) {
                smf_event_t* event_off = smf_event_new_from_bytes(0x80, notes[j], piano_volume);
                smf_track_add_event_seconds(piano_track, event_off, time_seconds);
            }
        }
    }

    if (smf_save(smf, filename) != 0) {
        fprintf(stderr, "Errore durante il salvataggio del file MIDI: %s\n", filename);
    }
    smf_delete(smf);
}
