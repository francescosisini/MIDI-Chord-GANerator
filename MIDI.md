# 🎵 MIDI: struttura, eventi e formato binario

Il formato **MIDI (Musical Instrument Digital Interface)** non contiene suoni reali, ma una sequenza di **eventi temporali** che descrivono *cosa* deve essere suonato, *quando* e *come*.  
È un linguaggio universale che permette a strumenti, software e sintetizzatori di comunicare tra loro.

---

## 🧩 Struttura di un file MIDI

Un file `.mid` è composto da:

1. **Header (testata)**
   - Specifica il **formato** del file (0, 1 o 2)
   - Il **numero di tracce**
   - La **risoluzione temporale**, cioè quanti "tick" corrispondono a un quarto di nota (es. 480)

2. **Tracce**
   - Ogni traccia è una sequenza di **eventi MIDI ordinati nel tempo**
   - Gli eventi vengono registrati con un **delta time**, ossia il numero di tick trascorsi dal precedente evento

---

## ⏱️ Tempo e Delta Time

Gli eventi non memorizzano il tempo assoluto, ma solo quanto tempo passa dal precedente evento.

Esempio:
Delta=0 NoteOn (C4, velocity=100)
Delta=480 NoteOff (C4)

➡️ significa che la nota **C4** inizia subito e dura un quarto (480 tick).

Il tempo effettivo in secondi dipende da un **meta-evento** chiamato `Set Tempo`, che indica quanti microsecondi dura un quarto di nota (ad esempio, 500000 µs corrispondono a 120 BPM).

---

## 🎹 Tipi principali di eventi MIDI

| Tipo evento | Codice (hex) | Descrizione |
|--------------|--------------|-------------|
| **Note On** | `0x9n` | Avvia una nota su canale `n` (pitch + velocity) |
| **Note Off** | `0x8n` | Termina una nota |
| **Control Change** | `0xBn` | Cambia un controllo (volume, sustain, pan...) |
| **Program Change** | `0xCn` | Cambia strumento sul canale |
| **Pitch Bend** | `0xEn` | Modifica temporaneamente l’intonazione |
| **Meta Event** | `0xFF` | Informazioni non musicali (tempo, testo, fine traccia...) |
| **SysEx** | `0xF0` / `0xF7` | Messaggi di sistema per sintetizzatori specifici |

---

## 🎼 Esempio logico di una traccia MIDI

Ecco un frammento di una traccia che suona un accordo di **Do maggiore (C-E-G)**:

MTrk
0 ProgramChange ch=0 prog=1 ; Pianoforte
0 NoteOn ch=0 note=60 vel=100 ; C4
0 NoteOn ch=0 note=64 vel=100 ; E4
0 NoteOn ch=0 note=67 vel=100 ; G4
480 NoteOff ch=0 note=60 vel=64
0 NoteOff ch=0 note=64 vel=64
0 NoteOff ch=0 note=67 vel=64
0 EndOfTrack


---

## 💾 Dal modello logico al file binario

Internamente, un file MIDI è **binario**, cioè una sequenza di byte che codificano gli eventi musicali in modo compatto.

Ogni evento ha questa struttura generale:

[delta time][status byte][data byte 1][data byte 2]


Esempio:

00 90 3C 64


| Byte | Significato |
|------|--------------|
| `00` | Delta time = 0 |
| `90` | Note On, canale 0 |
| `3C` | Pitch 60 (nota C4) |
| `64` | Velocity 100 |

Quando il delta time è diverso da 0, viene codificato come **Variable Length Quantity (VLQ)**:  
una sequenza di 1–4 byte, dove il bit più alto indica se ne seguono altri (`1` = continua, `0` = ultimo byte).

Esempio:  
`83 60` → delta time di 480 tick.

---

## 🧱 Struttura binaria completa

Un file `.mid` contiene:

4D 54 68 64 ; "MThd" (header chunk)
00 00 00 06 ; lunghezza header (6 byte)
00 01 ; formato (type 1 = multitrack)
00 01 ; numero tracce
01 E0 ; risoluzione (480 tick per quarto)

4D 54 72 6B ; "MTrk" (track chunk)
00 00 00 XX ; lunghezza dati della traccia
<eventi MIDI codificati in binario>
FF 2F 00 ; End of Track
