#ifndef VOICERECOGNITION_H
#define VOICERECOGNITION_H

#include "portaudio.h"

class VoiceRecognition
{
    public:
        VoiceRecognition();
        int Record(void);

    protected:
    private:
        static int recordCallback(const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData);
};

#endif // VOICERECOGNITION_H
