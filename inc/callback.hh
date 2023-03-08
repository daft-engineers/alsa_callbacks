#ifndef ALSA_CALLBACKS_HH
#define ALSA_CALLBACKS_HH

#include <alsa/asoundlib.h>
#include <alsa/error.h>
#include <alsa/pcm.h>
#include <cstdint>
#include <functional>
#include <iostream>
#include <sys/types.h>
#include <thread>
#include <vector>

namespace alsa_callback {

enum stream_type {
    playback = SND_PCM_STREAM_PLAYBACK,
    capture = SND_PCM_STREAM_CAPTURE
};

enum format {
    unsigned_16bit_little_endian = SND_PCM_FORMAT_U16_LE,
    float_32bit_little_endian = SND_PCM_FORMAT_FLOAT_LE
};

class acb {
public:
    acb(format format);
    void listen(const std::function<void (std::vector<uint16_t>)>& callback);
    void stop();

private:
    std::thread cb_thread;
    snd_pcm_hw_params_t* params {};
    snd_pcm_t* handle {};
    bool keep_listening {false};
    snd_pcm_uframes_t frames = 32; // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later

};

} // namespace alsa_callback

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#endif // ALSA_CALLBACKS_HH
