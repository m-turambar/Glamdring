#ifndef GLAMDRING_DAC_H
#define GLAMDRING_DAC_H

#include "helpers.h"
#include "RCC.h"

// Implementación inicial usa solo 1 canal, sin DMA
struct DAC
{
    enum class Canal {
        R,
        L
    };
    enum class Alignment {
        R12,
        L12,
        R8
    };
    enum class Wave {
        None,
        Noise,
        Triangle
    };
    enum class Trigger {
        Timer6,
        Timer8,
        Timer7,
        Timer5,
        Timer2,
        Timer4,
        ExternalLine9,
        Software
    };
    struct Config {
        Canal canal;
        uint8_t amplitud; // 4 bits, MAMP
        Wave wave;
        Trigger trigger;
        bool triggerEnable;
        bool outputBufferDisable;
    };

    const size_t base { 0x40007400 };
    registro CR, SWTRIGR, DHR12R1, DHR12L1, DHR8R1, DHR12R2, DHR12L2, DHR8R2, DHR12RD, DHR12LD, DHR8RD, DOR1, DOR2, SR;
    Config config;

    void enable() { }
    void disable() { }
    void sw_trigger() { }
};


#endif GLAMDRING_DAC_H