# 🎹 MIDI su Linux – Collegamento di una Tastiera a un Server MIDI

## Cos'è il MIDI
**MIDI** (*Musical Instrument Digital Interface*) è un protocollo che non trasporta suono, ma **eventi musicali digitali** — come “nota premuta”, “nota rilasciata”, “volume”, “cambio strumento”.  
Una tastiera MIDI invia questi messaggi a un **sintetizzatore software** (server MIDI), che li trasforma in suono udibile.

---

## 🎛 Requisiti
Assicurati di avere installati i pacchetti fondamentali:

```bash
sudo apt update
sudo apt install fluidsynth fluid-soundfont-gm alsa-utils
```

🔧 Verifica della tastiera MIDI

Collega la tastiera e verifica che venga riconosciuta:
```bash
aconnect -l

```

Esempio di output:
```bash
client 24: 'Keystation Mini 32'
    0 'Keystation Mini 32 MIDI 1'
client 128: 'FLUID Synth (47130)'
    0 'Synth input port (47130:0)'

```

🔊 Avvio del server MIDI (FluidSynth)

Avvia FluidSynth specificando una soundfont General MIDI:
```bash
fluidsynth -a pipewire /usr/share/sounds/sf2/FluidR3_GM.sf2
```

Alternative:

- pulseaudio → per sistemi con PulseAudio

- alsa → per uscita diretta ALSA

🔗 Connessione tra tastiera e sintetizzatore

In un altro terminale, collega la tastiera al sintetizzatore:
```bash
aconnect "Keystation Mini 32":0 "FLUID Synth":0
```

Verifica la connessione:
```bash
aconnect -l
```

🎹 Test del segnale MIDI

Per vedere se la tastiera invia dati:
```bash
aseqdump -p "Keystation Mini 32"
```

🎵 Riproduzione di file MIDI

Una volta che FluidSynth è attivo, puoi riprodurre un file .mid:
```bash
aplaymidi -p 128:0 my_song.mid

```
