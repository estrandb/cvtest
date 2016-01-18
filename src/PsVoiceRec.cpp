#include "PsVoiceRec.h"

#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include "pocketsphinx.h"

#include <stdio.h>
#include <stdlib.h>

#include "TextToSpeechController.h"

#define MODELDIR "/home/pi/builds/pocketsphinx-5prealpha/model"
#define KEYWORDDIR "/home/pi/projects/git/cvtest/keywords"

PsVoiceRec::PsVoiceRec(){};

int PsVoiceRec::ListenForKeyword()
{
    TextToSpeechController textToSpeechController;

    ps_decoder_t *ps;
    cmd_ln_t *config;
    ad_rec_t *ad;
    int16 adbuf[2048];
    uint8 utt_started, in_speech;
    int32 k;
    char const *hyp;

    config = cmd_ln_init(NULL, ps_args(), TRUE,
            "-hmm", MODELDIR "/en-us/en-us",
            //"-lm", MODELDIR "/en-us/en-us.lm.bin",
            "-dict", MODELDIR "/en-us/cmudict-en-us.dict",
            "-kws", KEYWORDDIR "/keys",
            "-kws_threshold", "1e-5",
            NULL);

    //ps_default_search_args(config);
    ps = ps_init(config);
    if (ps == NULL) {
        cmd_ln_free_r(config);
        return 1;
    }

    if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
                          (int) cmd_ln_float32_r(config,
                                                 "-samprate"))) == NULL)
        E_FATAL("Failed to open audio device\n");

    if (ad_start_rec(ad) < 0)
        E_FATAL("Failed to start recording\n");

    if (ps_start_utt(ps) < 0)
        E_FATAL("Failed to start utterance\n");
    utt_started = FALSE;
    printf("READY....\n");

    for (;;) {
        if ((k = ad_read(ad, adbuf, 2048)) < 0)
            E_FATAL("Failed to read audio\n");
        ps_process_raw(ps, adbuf, k, FALSE, FALSE);
        in_speech = ps_get_in_speech(ps);
        if (in_speech && !utt_started) {
            utt_started = TRUE;
            printf("Listening...\n");
        }
        if (!in_speech && utt_started) {
            /* speech -> silence transition, time to start new utterance  */
            ps_end_utt(ps);
            hyp = ps_get_hyp(ps, NULL );
            if (hyp != NULL)
            {
                printf("%s\n", hyp);
                textToSpeechController.RespondToKeyword(hyp);
            }
            if (ps_start_utt(ps) < 0)
                E_FATAL("Failed to start utterance\n");
            utt_started = FALSE;
            printf("READY....\n");
        }
        //usleep(100000);
    }
    ad_close(ad);
    return 1;
}
