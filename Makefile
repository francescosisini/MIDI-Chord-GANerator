# Nome del file eseguibile
TARGET = MelodyCraft

# File sorgenti
SRC = main.c chords.c melody.c key_detection.c melody_generator.c

# Percorsi di inclusione per le librerie
INCLUDES = -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

# Librerie da collegare
LIBS = -lsmf -lglib-2.0

# Compilatore
CC = gcc

# Opzioni del compilatore
CFLAGS = $(INCLUDES) $(LIBS)

# Regola predefinita
all: $(TARGET)

# Regola per creare l'eseguibile
$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

# Regola per installare le dipendenze
install-dependencies:
	sudo apt-get install libsmf-dev libglib2.0-dev

# Regola per pulire i file generati
clean:
	rm -f $(TARGET)

# Regola per eseguire il programma
run: $(TARGET)
	./$(TARGET)
