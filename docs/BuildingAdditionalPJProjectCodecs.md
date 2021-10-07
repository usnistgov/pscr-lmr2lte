Building additional PJProject codecs
```
cd ~/src/pjproject/pjlib/include/pj
... edit config_site.h to have
#define PJMEDIA_HAS_G7221_CODEC 1
#define PJMEDIA_CODEC_L16_HAS_8KHZ_MONO	1
#define PJMEDIA_CODEC_L16_HAS_8KHZ_STEREO	1
#define PJMEDIA_CODEC_L16_HAS_16KHZ_MONO 1
#define PJMEDIA_CODEC_L16_HAS_16KHZ_STEREO	1
#define PJMEDIA_CODEC_L16_HAS_48KHZ_MONO	1
#define PJMEDIA_CODEC_L16_HAS_48KHZ_STEREO	1
```
Default included codecs from included install procedure:
```
13 codecs found
1: audio payload type 98 speex 32000Hz 1 channels
2: audio payload type 97 speex 16000Hz 1 channels
3: audio payload type 96 speex 8000Hz 1 channels
4: audio payload type 103 iLBC 8000Hz 1 channels
5: audio payload type 3 GSM 8000Hz 1 channels
6: audio payload type 0 PCMU 8000Hz 1 channels
7: audio payload type 8 PCMA 8000Hz 1 channels
8: audio payload type 9 G722 16000Hz 1 channels
9: audio payload type 11 L16 44100Hz 1 channels
10: audio payload type 10 L16 44100Hz 2 channels
11: audio payload type 104 AMR 8000Hz 1 channels
12: audio payload type 105 AMR-WB 16000Hz 1 channels
13: audio payload type 119 opus 48000Hz 2 channels
```
After adding to config_site.h
```
24 codecs found
1: audio payload type 98 speex 32000Hz 1 channels
2: audio payload type 97 speex 16000Hz 1 channels
3: audio payload type 96 speex 8000Hz 1 channels
4: audio payload type 103 iLBC 8000Hz 1 channels
5: audio payload type 3 GSM 8000Hz 1 channels
6: audio payload type 0 PCMU 8000Hz 1 channels
7: audio payload type 8 PCMA 8000Hz 1 channels
8: audio payload type 9 G722 16000Hz 1 channels
9: audio payload type 112 G7221 16000Hz 1 channels
10: audio payload type 113 G7221 16000Hz 1 channels
11: audio payload type 114 G7221 32000Hz 1 channels
12: audio payload type 115 G7221 32000Hz 1 channels
13: audio payload type 116 G7221 32000Hz 1 channels
14: audio payload type 11 L16 44100Hz 1 channels
15: audio payload type 10 L16 44100Hz 2 channels
16: audio payload type 120 L16 8000Hz 1 channels
17: audio payload type 121 L16 8000Hz 2 channels
18: audio payload type 122 L16 16000Hz 1 channels
19: audio payload type 123 L16 16000Hz 2 channels
20: audio payload type 124 L16 48000Hz 1 channels
21: audio payload type 125 L16 48000Hz 2 channels
22: audio payload type 104 AMR 8000Hz 1 channels
23: audio payload type 105 AMR-WB 16000Hz 1 channels
24: audio payload type 119 opus 48000Hz 2 channels
```