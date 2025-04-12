#ifndef GLAMDRING_DAC_H
#define GLAMDRING_DAC_H

#include "helpers.h"
#include "RCC.h"

// Implementación inicial usa solo 1 canal, sin DMA
struct DAC
{
    enum class Canal {
        CH1, // PA4
        CH2  // PA5
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
        uint8_t amplitud; // Usa solo 4 de los 8 bits, bitfield MAMP
        Wave wave;
        Trigger trigger;
        bool triggerEnable;
        bool outputBufferDisable;
    };

    const size_t base { 0x40007400 };
    registro CR, SWTRIGR, DHR12R1, DHR12L1, DHR8R1, DHR12R2, DHR12L2, DHR8R2, DHR12RD, DHR12LD, DHR8RD, DOR1, DOR2, SR;
    Canal canal;

    DAC(const Config&);
    void enable();
    void disable();
    void trigger();
    void write_12R(uint16_t data);
};


// El DAC funciona particularmente bien con los timers TIM6 y TIM7.
// Si configuras CR2 en estos timers, puedes triggerear el DAC sin necesidad de interrupciones. :)
// Ejemplo de uso:
//
// DAC::Config dac_config
// {
//   DAC::Canal::CH1,
//   0x0B, // Amplitud (MAMP)
//   DAC::Wave::Triangle,
//   // DAC::Trigger::Software, // Requiere una interrupción
//   DAC::Trigger::Timer6,
//   true, // TEN
//   false, // BOFF disable
// };

// DAC dac(dac_config);
// dac.enable();
// basic_timer t6(BasicTimer::TIM6, basic_timer::Mode::Periodic);
// t6.configurar_periodo_us(10);
// t6.configure_master_mode(basic_timer::MasterMode::Update); // Para triggerear DAC
// t6.generate_update();
// t6.clear_update();
// t6.start();


#endif // GLAMDRING_DAC_H