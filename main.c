#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <smf.h>
#include "chords.h"
#include "melody.h"
#include "key_detection.h"

#define MAX_CHORDS 100
#define MAX_SECTIONS 10
#define MAX_LENGTH 10

// Globals
double SEMIMINIMA;           // Semiminima (1/4)
double MINIMA;               // Minima (1/2)
double CROMA;                // Croma (1/8)
double SEMICROMA;            // Semicroma (1/16)
double SEMIBREVE;            // Semibreve (intera)

typedef enum {
    INTRO,
    VERSO,
    RITORNELLO,
    PONTE,
    FINALE,
    NUM_SECTION_TYPES
} SectionType;

const char* section_names[] = {
    "INTRO",
    "VERSO",
    "RITORNELLO",
    "PONTE",
    "FINALE"
};

SectionType get_section_type(const char* name) {
    for (int i = 0; i < NUM_SECTION_TYPES; i++) {
        if (strcmp(name, section_names[i]) == 0) {
            return i;
        }
    }
    return NUM_SECTION_TYPES;
}


void write_midi_with_melody(char sections[MAX_SECTIONS][MAX_LENGTH], int section_repeats[MAX_SECTIONS], char section_chords[MAX_SECTIONS][MAX_CHORDS][MAX_LENGTH], int section_durations[MAX_SECTIONS][MAX_CHORDS], int song_structure[MAX_CHORDS][2], int song_structure_length, int beats_per_measure, int include_bass, int include_drums, int generate_melody, int melody_follow_key, int piano_volume, int drum_volume, int bass_volume, int melody_volume, const char* filename);
void get_scale_notes(const char* key, int* scale_notes, int* num_notes);

void print_help() {
    printf("Usage: melody_generator [options] [section] chord duration ... [section] chord duration ... [structure] section repeat ...\n");
    printf("Options:\n");
    printf("  -t BPM            Set the BPM (default: 90)\n");
    printf("  -m meter          Set the meter (3 or 4, default: 4)\n");
    printf("  -r repeats        Set the number of repeats (default: 1)\n");
    printf("  -b                Include bass\n");
    printf("  -d                Include drums\n");
    printf("  -g                Generate melody\n");
    printf("  -k                Follow key for melody\n");
    printf("  -p piano_volume   Set the piano volume (0-127, default: 100)\n");
    printf("  -v drum_volume    Set the drum volume (0-127, default: 100)\n");
    printf("  -q bass_volume    Set the bass volume (0-127, default: 100)\n");
    printf("  -w melody_volume  Set the melody volume (0-127, default: 100)\n");
    printf("  -o filename       Set the output filename (default: output.mid)\n");
    printf("  -h                Show this help message\n");
}

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
    char sections[MAX_SECTIONS][MAX_LENGTH];
    int section_repeats[MAX_SECTIONS];
    char section_chords[MAX_SECTIONS][MAX_CHORDS][MAX_LENGTH];
    int section_durations[MAX_SECTIONS][MAX_CHORDS];
    int song_structure[MAX_CHORDS][2];
    int section_index = -1;
    int song_structure_length = 0;
    char output_filename[100] = "output.mid";

    int opt;
    while ((opt = getopt(argc, argv, "t:m:r:bdgkp:v:q:w:o:h")) != -1) {
        switch (opt) {
            case 't':
                BPM = atoi(optarg);
                if (BPM < 1 || BPM > 200) {
                    fprintf(stderr, "Invalid BPM. Must be between 1 and 200. Defaulting to 90.\n");
                    BPM = 90;
                }
                break;
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
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }

    // Eval notes duration
    SEMIMINIMA = 60.0 / BPM;           // Semiminima (1/4)
    MINIMA = 2 * SEMIMINIMA;           // Minima (1/2)
    CROMA = SEMIMINIMA / 2;            // Croma (1/8)
    SEMICROMA = SEMIMINIMA / 4;        // Semicroma (1/16)
    SEMIBREVE = 4 * SEMIMINIMA;        // Semibreve (intera)

    int chord_index = 0;
    for (int i = optind; i < argc; i++) {
        if (argv[i][0] == '[' && strcmp(argv[i], "[CANZONE]") != 0) {
            section_index++;
            if (section_index >= MAX_SECTIONS) {
                fprintf(stderr, "Exceeded maximum number of sections.\n");
                exit(EXIT_FAILURE);
            }
            strcpy(sections[section_index], argv[i]);
            section_repeats[section_index] = 1; // Default to 1 repeat if not specified
            chord_index = 0;
        } else if (isdigit(argv[i][0]) || (argv[i][0] == '-' && isdigit(argv[i][1]))) {
            section_repeats[section_index] = atoi(argv[i]);
        } else if (strcmp(argv[i], "[CANZONE]") == 0) {
            while (i + 2 < argc && song_structure_length < MAX_CHORDS) {
                SectionType section_type = get_section_type(argv[i + 1]);
                if (section_type != NUM_SECTION_TYPES) {
                    song_structure[song_structure_length][0] = section_type;
                    song_structure[song_structure_length][1] = atoi(argv[i + 2]);
                    song_structure_length++;
                } else {
                    fprintf(stderr, "Invalid section name: %s\n", argv[i + 1]);
                    exit(EXIT_FAILURE);
                }
                i += 2;
            }
            break;
	    
        } else {
            if (section_index == -1) {
                fprintf(stderr, "No sections defined before chords.\n");
                exit(EXIT_FAILURE);
            }
            if (chord_index >= MAX_CHORDS) {
                fprintf(stderr, "Exceeded maximum number of chords in a section.\n");
                exit(EXIT_FAILURE);
            }
            strcpy(section_chords[section_index][chord_index], argv[i]);
            section_durations[section_index][chord_index] = atoi(argv[i + 1]);
            chord_index++;
            i++;
        }
    }

    if (section_index == -1 || song_structure_length == 0) {
        fprintf(stderr, "No valid sections or song structure provided.\n");
        exit(EXIT_FAILURE);
    }

    printf("Sections:\n");
    for (int i = 0; i <= section_index; i++) {
        printf("Section %d (%s), repeats %d\n", i, sections[i], section_repeats[i]);
        for (int j = 0; j < MAX_CHORDS && section_chords[i][j][0] != 0; j++) {
            printf("  Chord %d: %s, duration: %d\n", j, section_chords[i][j], section_durations[i][j]);
        }
    }

    printf("Song structure:\n");
    for (int i = 0; i < song_structure_length; i++) {
        printf("  Section %d, repeats %d\n", song_structure[i][0], song_structure[i][1]);
    }

    write_midi_with_melody(sections, section_repeats, section_chords, section_durations, song_structure, song_structure_length, beats_per_measure, include_bass, include_drums, generate_melody, melody_follow_key, piano_volume, drum_volume, bass_volume, melody_volume, output_filename);

    printf("File MIDI generato: %s\n", output_filename);
    return 0;
}

void write_midi_with_melody(char sections[MAX_SECTIONS][MAX_LENGTH], int section_repeats[MAX_SECTIONS], char section_chords[MAX_SECTIONS][MAX_CHORDS][MAX_LENGTH], int section_durations[MAX_SECTIONS][MAX_CHORDS], int song_structure[MAX_CHORDS][2], int song_structure_length, int beats_per_measure, int include_bass, int include_drums, int generate_melody, int melody_follow_key, int piano_volume, int drum_volume, int bass_volume, int melody_volume, const char* filename) {
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

    for (int r = 0; r < song_structure_length; r++) {
        int section_index = song_structure[r][0];
        int repeat = song_structure[r][1];

        for (int j = 0; j < repeat; j++) {
            for (int i = 0; i < MAX_CHORDS && section_chords[section_index][i][0] != 0; i++) {
                int notes[10];
                int num_notes = chord_to_notes(section_chords[section_index][i], notes);

                // Aggiungi note dell'accordo al piano
                for (int k = 0; k < num_notes; k++) {
                    smf_event_t* note_on = smf_event_new_from_bytes(0x90 | 0x00, notes[k], piano_volume);
                    smf_track_add_event_seconds(piano_track, note_on, time_seconds);
                    smf_event_t* note_off = smf_event_new_from_bytes(0x80 | 0x00, notes[k], piano_volume);
                    smf_track_add_event_seconds(piano_track, note_off, time_seconds + section_durations[section_index][i] * SEMIMINIMA);
                }

                // Aggiungi melodia se richiesto
                if (generate_melody) {
                    if (melody_follow_key) {
                        get_scale_notes(section_chords[section_index][i], scale_notes, &num_scale_notes);
                    } else {
                        for (int k = 0; k < 128; k++) {
                            scale_notes[k] = k;
                        }
                        num_scale_notes = 128;
                    }
                    generate_varied_duration_melody(melody_track, num_notes, time_seconds, section_durations[section_index][i] * SEMIMINIMA, scale_notes, num_scale_notes, melody_volume);
                }

                // Aggiungi basso se richiesto
                if (include_bass) {
                    int bass_note = notes[0] - 12;
                    smf_event_t* bass_on = smf_event_new_from_bytes(0x90 | 0x02, bass_note, bass_volume);
                    smf_track_add_event_seconds(bass_track, bass_on, time_seconds);
                    smf_event_t* bass_off = smf_event_new_from_bytes(0x80 | 0x02, bass_note, bass_volume);
                    smf_track_add_event_seconds(bass_track, bass_off, time_seconds + section_durations[section_index][i] * SEMIMINIMA);
                }

                // Aggiungi batteria se richiesta
                if (include_drums) {
                    int kick_drum_note = 36;
                    smf_event_t* kick_drum_on = smf_event_new_from_bytes(0x99, kick_drum_note, drum_volume);
                    smf_track_add_event_seconds(drum_track, kick_drum_on, time_seconds);
                    smf_event_t* kick_drum_off = smf_event_new_from_bytes(0x89, kick_drum_note, drum_volume);
                    smf_track_add_event_seconds(drum_track, kick_drum_off, time_seconds + SEMIMINIMA);

                    int snare_drum_note = 38;
                    smf_event_t* snare_drum_on = smf_event_new_from_bytes(0x99, snare_drum_note, drum_volume);
                    smf_track_add_event_seconds(drum_track, snare_drum_on, time_seconds + MINIMA);
                    smf_event_t* snare_drum_off = smf_event_new_from_bytes(0x89, snare_drum_note, drum_volume);
                    smf_track_add_event_seconds(drum_track, snare_drum_off, time_seconds + MINIMA + SEMIMINIMA);

                    int hi_hat_note = 42;
                    for (int beat = 0; beat < beats_per_measure; beat++) {
                        smf_event_t* hi_hat_on = smf_event_new_from_bytes(0x99, hi_hat_note, drum_volume);
                        smf_track_add_event_seconds(drum_track, hi_hat_on, time_seconds + beat * SEMIMINIMA);
                        smf_event_t* hi_hat_off = smf_event_new_from_bytes(0x89, hi_hat_note, drum_volume);
                        smf_track_add_event_seconds(drum_track, hi_hat_off, time_seconds + beat * SEMIMINIMA + CROMA);
                    }
                }

                time_seconds += section_durations[section_index][i] * SEMIMINIMA;
                if (time_seconds < 0) {
                    fprintf(stderr, "Invalid time_seconds calculated: %f\n", time_seconds);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    int result = smf_save(smf, filename);
    if (result != 0) {
        fprintf(stderr, "Errore nel salvataggio del file MIDI: %s\n", filename);
    }

    smf_delete(smf);
}
