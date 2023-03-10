#include "../inc/callback.hh"

alsa_callback::acb::acb(alsa_callback::format format) {
    int errorcode = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    int dir = 0; // *_near() functions use this to say if the chosen value was above or below the requested value
    unsigned int sample_rate = 44100; // NOLINT(cppcoreguidelines-avoid-magic-numbers) this is modified in place later

    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to open pcm device: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    } else {
        std::cerr << "opened pcm device" << std::endl;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast) don't lint problems caused by external libraries
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);

    std::cerr << "allocced param device" << std::endl;

    errorcode = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set access: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    } else {
        std::cerr << "set access" << std::endl;
    }

    errorcode = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set format: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    } else {
        std::cerr << "set format" << std::endl;
    }

    errorcode = snd_pcm_hw_params_set_channels(handle, params, 2);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set channels: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    } else {
        std::cerr << "set channels" << std::endl;
    }

    errorcode = snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set rate: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    } else {
        std::cerr << "set rate" << std::endl;
    }

    errorcode = snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set size: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    } else {
        std::cerr << "set size" << std::endl;
    }

    std::cerr << "set params in params" << std::endl;

    errorcode = snd_pcm_hw_params(handle, params);
    if (errorcode < 0) {
        std::cerr << "alsa_callback: unable to set hw paramters: " << snd_strerror(errorcode) << "\n";
        std::quick_exit(1);
    } else {
        std::cerr << "wrote params to hw" << std::endl;
    }
}

void alsa_callback::acb::listen(const std::function<void(std::vector<uint16_t>)> &callback) {
    keep_listening = true;
    std::cerr << "in listen function\n";
    cb_thread = std::thread([this, &callback]() {
        std::cerr << "inside thread\n";

        while (this->keep_listening) {
            std::vector<uint16_t> buffer{};
            buffer.resize(this->frames * 2);

            // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions) error comes from ALSA library
            int errorcode = snd_pcm_readi(this->handle, &buffer[0], this->frames);

            if (errorcode == -EPIPE) {
                std::cerr << "alsa_callback: Overrun occurred\n";
                snd_pcm_prepare(this->handle);
            } else if (errorcode < 0) {
                std::cerr << "alsa_callback: Error from read " << snd_strerror(errorcode);
            } else if (errorcode != (int)this->frames) {
                std::cerr << "alsa_callback: short read, read " << errorcode << " frames\n";
            }
            // std::cerr << "data collected, passing vector to cb\n";

            callback(buffer);
            // std::cerr << "after callback call\n";
        }

        std::cerr << "keep listening is false, exiting thread\n";
    });
    std::cerr << "thread created, exiting listen function\n";
}

void alsa_callback::acb::stop() {
    std::cerr << "entering stop function\n";
    keep_listening = false;
    std::cerr << "set keep_listening to false\n";
    cb_thread.join();
    std::cerr << "joined thread\n";
    snd_pcm_drop(handle);
    std::cerr << "pcm drained\n";
    snd_pcm_close(handle);
    std::cerr << "pcm closed\n";
}
