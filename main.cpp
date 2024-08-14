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
    float frequencies[17];
    float phases[17];
    float amplitude;
    unsigned long framesLeft[17];
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

        for (int j = 0; j < 17; j++) {
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
    keymap["z"] = 122;
    keymap["x"] = 120;
    keymap["c"] = 99;
    keymap["v"] = 118;
    keymap["b"] = 98;
    keymap["n"] = 110;
    keymap["m"] = 109;

    std::map<int, std::string> tonemap;
    tonemap[keymap["1"]] = "C4";
    tonemap[keymap["2"]] = "D4";
    tonemap[keymap["3"]] = "E4";
    tonemap[keymap["4"]] = "F4";
    tonemap[keymap["5"]] = "G4";
    tonemap[keymap["6"]] = "A4";
    tonemap[keymap["7"]] = "B4";
    tonemap[keymap["8"]] = "C5";
    tonemap[keymap["9"]] = "D5";
    tonemap[keymap["0"]] = "E5";
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
    tonemap[keymap["z"]] = "C3";
    tonemap[keymap["x"]] = "D3";
    tonemap[keymap["c"]] = "E3";
    tonemap[keymap["v"]] = "F3";
    tonemap[keymap["b"]] = "G3";
    tonemap[keymap["n"]] = "A3";
    tonemap[keymap["m"]] = "B3";

    std::map<std::string, int> indexmap;
    indexmap["C3"] = 0;
    indexmap["D3"] = 1;
    indexmap["E3"] = 2;
    indexmap["F3"] = 3;
    indexmap["G3"] = 4;
    indexmap["A3"] = 5;
    indexmap["B3"] = 6;
    indexmap["C4"] = 7;
    indexmap["D4"] = 8;
    indexmap["E4"] = 9;
    indexmap["F4"] = 10;
    indexmap["G4"] = 11;
    indexmap["A4"] = 12;
    indexmap["B4"] = 13;
    indexmap["C5"] = 14;
    indexmap["D5"] = 15;
    indexmap["E5"] = 16;

   
    PaError err;
    SineWaveData data;
    
    data.frequencies[indexmap["C3"]] = 130.82;
    data.frequencies[indexmap["D3"]] = 146.83;
    data.frequencies[indexmap["E3"]] = 164.82;
    data.frequencies[indexmap["F3"]] = 174.62;
    data.frequencies[indexmap["G3"]] = 196.00;
    data.frequencies[indexmap["A3"]] = 220.00;
    data.frequencies[indexmap["B3"]] = 246.94;
    data.frequencies[indexmap["C4"]] = 261.63;
    data.frequencies[indexmap["D4"]] = 293.66;
    data.frequencies[indexmap["E4"]] = 329.63;
    data.frequencies[indexmap["F4"]] = 349.23;
    data.frequencies[indexmap["G4"]] = 392.00;
    data.frequencies[indexmap["A4"]] = 440.00;
    data.frequencies[indexmap["B4"]] = 493.88;
    data.frequencies[indexmap["C5"]] = 523.25;
    data.frequencies[indexmap["D5"]] = 587.33;
    data.frequencies[indexmap["E5"]] = 659.26;


    for (int i = 0; i < 17; i++) {
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

