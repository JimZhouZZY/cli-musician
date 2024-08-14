#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <math.h>
#include <portaudio.h>
#include <ncurses.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512
#define SOUND_DURATION_FRAMES (SAMPLE_RATE * 1.0) // 0.5 second duration

typedef struct
{
    float frequencies[29];
    float phases[29];
    float amplitude;
    unsigned long framesLeft[29];
} SineWaveData;

int sineWaveCallback(const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData)
{
    SineWaveData *data = (SineWaveData *)userData;
    float *out = (float *)outputBuffer;
    (void) inputBuffer; /* Prevent unused variable warning. */

    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        float sample = 0.0;

        for (int j = 0; j < 29; j++) {
            if (data->framesLeft[j] > 0) {
                float volume = (float)data->framesLeft[j] / SOUND_DURATION_FRAMES; // Calculate fading volume
                sample += volume * data->amplitude * sinf(2.0f * M_PI * data->phases[j]);
                data->phases[j] += data->frequencies[j] / SAMPLE_RATE;
                if (data->phases[j] >= 1.0) data->phases[j] -= 1.0;
                data->framesLeft[j]--;
            }
        }

        *out++ = sample * 0.5; // Scale down the mixed sample
    }

    return paContinue;
}

void initNcurses()
{
    initscr();            // Initialize ncurses
    cbreak();             // Disable line buffering
    noecho();             // Don't display typed characters
    keypad(stdscr, TRUE); // Enable special keys (e.g., arrow keys)
}

void cleanupNcurses()
{
    endwin(); // End ncurses mode
}

int main(void)
{
     
    std::map<std::string, int> keymap;
    // LINE 0
    keymap["esc"] = 27;
    keymap["0"] = 48;
    keymap["1"] = 49;
    keymap["2"] = 50;
    keymap["3"] = 51;
    keymap["4"] = 52;
    keymap["5"] = 53;
    keymap["6"] = 54;
    keymap["7"] = 55;
    keymap["8"] = 56;
    keymap["9"] = 57;
    // LINE 1
    keymap["q"] = 113;
    keymap["w"] = 119;
    keymap["e"] = 101;
    keymap["r"] = 114;
    keymap["t"] = 116;
    keymap["y"] = 121;
    keymap["u"] = 117;
    keymap["i"] = 105;
    keymap["o"] = 111;
    keymap["p"] = 112;
    // LINE 2
    keymap["a"] = 97;
    keymap["s"] = 115;
    keymap["d"] = 100;
    keymap["f"] = 102;
    keymap["g"] = 103;
    keymap["h"] = 104;
    keymap["j"] = 106;
    keymap["k"] = 107;
    keymap["l"] = 108;
    // LINE 3
    keymap["z"] = 122;
    keymap["x"] = 120;
    keymap["c"] = 99;
    keymap["v"] = 118;
    keymap["b"] = 98;
    keymap["n"] = 110;
    keymap["m"] = 109;

    std::map<int, std::string> tonemap;
    // LINE 0
    // tonemap[keymap["1"]] = "C4";
    tonemap[keymap["2"]] = "C#4";
    tonemap[keymap["3"]] = "D#4";
    // tonemap[keymap["4"]] = "F4";
    tonemap[keymap["5"]] = "F#4";
    tonemap[keymap["6"]] = "G#4";
    tonemap[keymap["7"]] = "A#4";
    // tonemap[keymap["8"]] = "C5";
    tonemap[keymap["9"]] = "C#5";
    tonemap[keymap["0"]] = "D#5";
    // LINE 1
    tonemap[keymap["q"]] = "C4";
    tonemap[keymap["w"]] = "D4";
    tonemap[keymap["e"]] = "E4";
    tonemap[keymap["r"]] = "F4";
    tonemap[keymap["t"]] = "G4";
    tonemap[keymap["y"]] = "A4";
    tonemap[keymap["u"]] = "B4";
    tonemap[keymap["i"]] = "C5";
    tonemap[keymap["o"]] = "D5";
    tonemap[keymap["p"]] = "E5";
    // LINE 2
    tonemap[keymap["s"]] = "C#3";
    tonemap[keymap["d"]] = "D#3";
    tonemap[keymap["g"]] = "F#3";
    tonemap[keymap["h"]] = "G#3";
    tonemap[keymap["j"]] = "A#3";
    // LINE 3
    tonemap[keymap["z"]] = "C3";
    tonemap[keymap["x"]] = "D3";
    tonemap[keymap["c"]] = "E3";
    tonemap[keymap["v"]] = "F3";
    tonemap[keymap["b"]] = "G3";
    tonemap[keymap["n"]] = "A3";
    tonemap[keymap["m"]] = "B3";

    std::map<std::string, int> indexmap;
    indexmap["C3"] = 0;
    indexmap["C#3"] = 1;
    indexmap["D3"] = 2;
    indexmap["D#3"] = 3;
    indexmap["E3"] = 4;
    indexmap["F3"] = 5;
    indexmap["F#3"] = 6;
    indexmap["G3"] = 7;
    indexmap["G#3"] = 8;
    indexmap["A3"] = 9;
    indexmap["A#3"] = 10;
    indexmap["B3"] = 11;
    indexmap["C4"] = 12;
    indexmap["C#4"] = 13;
    indexmap["D4"] = 14;
    indexmap["D#4"] = 15;
    indexmap["E4"] = 16;
    indexmap["F4"] = 17;
    indexmap["F#4"] = 18;
    indexmap["G4"] = 19;
    indexmap["G#4"] = 20;
    indexmap["A4"] = 21;
    indexmap["A#4"] = 22;
    indexmap["B4"] = 23;
    indexmap["C5"] = 24;
    indexmap["C#5"] = 25;
    indexmap["D5"] = 26;
    indexmap["D#5"] = 27;
    indexmap["E5"] = 28;

   
    PaError err;
    SineWaveData data;
    
    // Frequencies of the sound (Hz).
    float ra = 1.05946;
    data.frequencies[indexmap["C3"]] = 130.82;
    data.frequencies[indexmap["C#3"]] = 130.82 * ra;
    data.frequencies[indexmap["D3"]] = 146.83;
    data.frequencies[indexmap["D#3"]] = 146.83 * ra;
    data.frequencies[indexmap["E3"]] = 164.82;
    data.frequencies[indexmap["F3"]] = 174.62;
    data.frequencies[indexmap["F#3"]] = 174.62 * ra;
    data.frequencies[indexmap["G3"]] = 196.00;
    data.frequencies[indexmap["G#3"]] = 196.00 * ra;
    data.frequencies[indexmap["A3"]] = 220.00;
    data.frequencies[indexmap["A#3"]] = 220.00 * ra;
    data.frequencies[indexmap["B3"]] = 246.94;
    data.frequencies[indexmap["C4"]] = 261.63;
    data.frequencies[indexmap["C#4"]] = 261.63 * ra;
    data.frequencies[indexmap["D4"]] = 293.66;
    data.frequencies[indexmap["D#4"]] = 293.66 * ra;
    data.frequencies[indexmap["E4"]] = 329.63;
    data.frequencies[indexmap["F4"]] = 349.23;
    data.frequencies[indexmap["F#4"]] = 349.23 * ra;
    data.frequencies[indexmap["G4"]] = 392.00;
    data.frequencies[indexmap["G#4"]] = 392.00 * ra;
    data.frequencies[indexmap["A4"]] = 440.00;
    data.frequencies[indexmap["A#4"]] = 440.00 * ra;
    data.frequencies[indexmap["B4"]] = 493.88;
    data.frequencies[indexmap["C5"]] = 523.25;
    data.frequencies[indexmap["C#5"]] = 523.25 * ra;
    data.frequencies[indexmap["D5"]] = 587.33;
    data.frequencies[indexmap["D#5"]] = 587.33 * ra;
    data.frequencies[indexmap["E5"]] = 659.26;


    for (int i = 0; i < 29; i++) {
        data.phases[i] = 0.0;
        data.framesLeft[i] = 0;
    }
    data.amplitude = 0.2; // Amplitude of the sine waves

    initNcurses();
    
    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        cleanupNcurses();
        return 1;
    }

    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream,
                               0,          // no input channels
                               1,          // mono output
                               paFloat32,  // 32-bit floating point output
                               SAMPLE_RATE,
                               FRAMES_PER_BUFFER,
                               sineWaveCallback,
                               &data);

    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        cleanupNcurses();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        cleanupNcurses();
        return 1;
    }

    printw("Press keys to play notes (q for C), and ESC to quit.\n");
    refresh();

    int ch;
    while ((ch = getch()) != keymap["esc"]) {
        printw("%d\n", ch);
        int index = indexmap[tonemap[ch]];
        data.framesLeft[index] = SOUND_DURATION_FRAMES; // Set duration for the corresponding note
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    }

    Pa_Terminate();
    cleanupNcurses();
    printw("Done.\n");
    refresh();
    return 0;
}

