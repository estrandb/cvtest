#ifndef VOICERECOGNITION_H
#define VOICERECOGNITION_H

#include <boost/thread.hpp>
#include "portaudio.h"
#include "pa_util.h"
#include "pa_ringbuffer.h"

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
        static int threadFunctionWriteToRawFile(void* ptr);
        static int threadFunctionReadFromRawFile(void* ptr);
    protected:
    private:


        /* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
        #define FILE_NAME       "audio_data.raw"
        #define SAMPLE_RATE  (44100)
        #define FRAMES_PER_BUFFER (512)
        #define NUM_SECONDS     (10)
        #define NUM_CHANNELS    (2)
        #define NUM_WRITES_PER_BUFFER   (4)
        /* #define DITHER_FLAG     (paDitherOff) */
        #define DITHER_FLAG     (0) /**/


        /* Select sample format. */
        #if 0
        #define PA_SAMPLE_TYPE  paFloat32
        typedef float SAMPLE;
        #define SAMPLE_SILENCE  (0.0f)
        #define PRINTF_S_FORMAT "%.8f"
        #elif 1
        #define PA_SAMPLE_TYPE  paInt16
        typedef short SAMPLE;
        #define SAMPLE_SILENCE  (0)
        #define PRINTF_S_FORMAT "%d"
        #elif 0
        #define PA_SAMPLE_TYPE  paInt8
        typedef char SAMPLE;
        #define SAMPLE_SILENCE  (0)
        #define PRINTF_S_FORMAT "%d"
        #else
        #define PA_SAMPLE_TYPE  paUInt8
        typedef unsigned char SAMPLE;
        #define SAMPLE_SILENCE  (128)
        #define PRINTF_S_FORMAT "%d"
        #endif


        typedef struct paTestDataStruct
        {
            unsigned            frameIndex;
            int                 threadSyncFlag;
            SAMPLE             *ringBufferData;
            PaUtilRingBuffer    ringBuffer;
            FILE               *file;
            void               *threadHandle;
        }
        paTestData;
        static PaError startThread(paTestData* pData);
        static int stopThread(paTestData* pData);

};

#endif // VOICERECOGNITION_H
