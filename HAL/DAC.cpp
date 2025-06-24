#include "DAC.h"
#include "RCC.h"

DAC::DAC(const DAC::Config& config)
    : CR(base)
    , SWTRIGR(base + 0x4)
    , DHR12R1(base + 0x8)
    , DHR12L1(base + 0xC)
    , DHR8R1(base + 0x10)
    , DHR12R2(base + 0x14)
    , DHR12L2(base + 0x18)
    , DHR8R2(base + 0x1C)
    , DHR12RD(base + 0x20)
    , DHR12LD(base + 0x24)
    , DHR8RD(base + 0x28)
    , DOR1(base + 0x2C)
    , DOR2(base + 0x30)
    , SR(base + 0x34)
{
    canal = config.canal;
    const uint8_t offset = canal == Canal::CH1 ? 0 : 16;
    bitfield MAMP(4, offset + 8, (config.amplitud & 0x0F));
    bitfield WAVE(2, offset + 6, static_cast<size_t>(config.wave));
    bitfield TSEL(3, offset + 3, static_cast<size_t>(config.trigger));
    flag TEN(offset + 2);
    flag BOFF(offset + 1);

    RCC::enable_DAC_clock();
    disable();

    CR.write(MAMP);
    CR.write(WAVE);
    CR.write(TSEL);
    if (config.triggerEnable)
        CR.set(TEN);
    else
        CR.reset(TEN);
    if (config.outputBufferDisable)
        CR.set(BOFF);
    else
        CR.reset(BOFF);
}

void DAC::enable()
{
    const uint8_t offset = canal == Canal::CH1 ? 0 : 16;
    flag EN(offset);
    CR.set(EN);
}

void DAC::disable()
{
    const uint8_t offset = canal == Canal::CH1 ? 0 : 16;
    flag EN(offset);
    CR.reset(EN);
}

void DAC::trigger()
{
    flag trig(canal == Canal::CH1 ? 0 : 1);
    SWTRIGR.set(trig);
}

void DAC::write_12R(uint16_t data)
{
    bitfield DACCxDHR(12, 0, data & 0x0FFF);
    if (canal == Canal::CH1)
        DHR12R1.write(DACCxDHR);
    else
        DHR12R2.write(DACCxDHR);
}