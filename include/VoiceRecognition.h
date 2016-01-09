#ifndef VOICERECOGNITION_H
#define VOICERECOGNITION_H

#include "portaudio.h"

class VoiceRecognition
{
    public:
        VoiceRecognition();
        int Record(void);
        static int recordCallback(const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData);
        static int recordTest( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData );
        static int playCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData );
        static int threadFunctionWriteToRawFile(void* ptr);
        static int threadFunctionReadFromRawFile(void* ptr);
    protected:
    private:

};

#endif // VOICERECOGNITION_H
