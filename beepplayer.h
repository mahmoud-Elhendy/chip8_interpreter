#include "miniaudio.h"
#include <cmath>
#include <iostream>

// beepPlayer class to encapsulate the Miniaudio device and state
class beepPlayer {
public:
    beepPlayer() : pDevice(nullptr), isPlaying(false) {}

    // Callback function to generate beep sound (sine wave)
    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, uint32_t frameCount) {
        static float phase = 0.0f;
        const float frequency = 440.0f;  // Frequency of the beep (440 Hz)
        const float sampleRate = 44100.0f;
        const float twoPi = 2.0f * (float)M_PI;
        float* output = (float*)pOutput;

        for (uint32_t i = 0; i < frameCount; i++) {
            // Generate sine wave for the beep
            float sample = 0.5f * sinf(phase);  // 0.5 is the volume level
            output[i * 2 + 0] = sample;  // Left channel
            output[i * 2 + 1] = sample;  // Right channel

            // Update phase for next sample
            phase += twoPi * frequency / sampleRate;
            if (phase > twoPi) {
                phase -= twoPi;
            }
        }
    }

    // Start the beep sound without blocking
    void start_beep() {
        if (!isPlaying) {
            ma_result result;

            // Configure the playback device
            ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
            deviceConfig.playback.format   = ma_format_f32;   // Floating point audio
            deviceConfig.playback.channels = 2;                // Stereo sound
            deviceConfig.sampleRate       = 44100;            // Standard sample rate
            deviceConfig.dataCallback     = data_callback;    // Use the sine wave callback

            pDevice = new ma_device();
            result = ma_device_init(nullptr, &deviceConfig, pDevice);
            if (result != MA_SUCCESS) {
                std::cerr << "Failed to initialize playback device" << std::endl;
                delete pDevice;
                pDevice = nullptr;
                return;
            }

            // Start playback
            result = ma_device_start(pDevice);
            if (result != MA_SUCCESS) {
                std::cerr << "Failed to start playback device" << std::endl;
                ma_device_uninit(pDevice);
                delete pDevice;
                pDevice = nullptr;
                return;
            }

            isPlaying = true;  // Start playing the beep
        }
    }

    // Stop the beep sound
    void stop_beep() {
        if (isPlaying) {
            ma_device_stop(pDevice);
            ma_device_uninit(pDevice);
            delete pDevice;
            pDevice = nullptr;
            isPlaying = false;  // Stop playing the beep
        }
    }

private:
    ma_device* pDevice;  // Device pointer to manage playback state
    bool isPlaying;      // Track playback status
};