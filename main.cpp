#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <portaudio.h>
#include <ncurses.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512
#define SOUND_DURATION_FRAMES (SAMPLE_RATE * 1.0) // 0.5 second duration

typedef struct
{
    float frequencies[9];
    float phases[9];
    float amplitude;
    unsigned long framesLeft[9];
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

        for (int j = 0; j < 9; j++) {
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
    PaError err;
    SineWaveData data;
    
    data.frequencies[0] = 261.63; // C
    data.frequencies[1] = 293.66; // D
    data.frequencies[2] = 329.63; // E
    data.frequencies[3] = 349.23; // F
    data.frequencies[4] = 392.00; // G
    data.frequencies[5] = 440.00; // A
    data.frequencies[6] = 493.88; // B    
    data.frequencies[7] = 523.25; // C (octave above)
    data.frequencies[8] = 587.33; // D (octave above)


    for (int i = 0; i < 9; i++) {
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

    printw("Press keys 1-9 to play notes CDEFGAB, and ESC to quit.\n");
    refresh();

    int ch;
    while ((ch = getch()) != 27) {
        printw("%d\n", ch);
        if (ch >= '1' && ch <= '9') {
            int index = ch - '1';
            data.framesLeft[index] = SOUND_DURATION_FRAMES; // Set duration for the corresponding note
        }
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

