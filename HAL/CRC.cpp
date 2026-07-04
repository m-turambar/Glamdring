#include "CRC.h"

static void aplicar_config(registro& CR_reg, registro& INIT, registro& POL, const CRC::Config& c)
{
    RCC::enable_CRC_clock();

    INIT.write(bitfield(32, 0, c.valor_inicial));
    POL.write(bitfield(32, 0, c.polinomio));

    const bitfield POLYSIZE(2, 3, static_cast<size_t>(c.tamanio_polinomio));
    const bitfield REV_IN(2, 5, static_cast<size_t>(c.rev_entrada));
    const flag REV_OUT(7);
    const flag RESET_bit(0);

    CR_reg.write(POLYSIZE);
    CR_reg.write(REV_IN);
    if (c.rev_salida)
        CR_reg.set(REV_OUT);
    else
        CR_reg.reset(REV_OUT);

    CR_reg.set(RESET_bit);
}

CRC::CRC()
    : DR(base)
    , IDR(base + 0x04)
    , CR_reg(base + 0x08)
    , INIT(base + 0x10)
    , POL(base + 0x14)
{
    aplicar_config(CR_reg, INIT, POL, Config{});
}

CRC::CRC(const Config& config)
    : DR(base)
    , IDR(base + 0x04)
    , CR_reg(base + 0x08)
    , INIT(base + 0x10)
    , POL(base + 0x14)
{
    aplicar_config(CR_reg, INIT, POL, config);
}

void CRC::reset()
{
    CR_reg.set(flag(0));
}

uint32_t CRC::calcular(uint32_t dato)
{
    memoria(DR) = dato;
    return resultado();
}

uint32_t CRC::calcular(uint16_t dato)
{
    *reinterpret_cast<volatile uint16_t*>(base) = dato;
    return resultado();
}

uint32_t CRC::calcular(uint8_t dato)
{
    *reinterpret_cast<volatile uint8_t*>(base) = dato;
    return resultado();
}

uint32_t CRC::calcular(const uint8_t* datos, size_t longitud)
{
    size_t i = 0;
    for (; i + 3 < longitud; i += 4) {
        uint32_t word = (static_cast<uint32_t>(datos[i])     << 24)
                      | (static_cast<uint32_t>(datos[i + 1]) << 16)
                      | (static_cast<uint32_t>(datos[i + 2]) <<  8)
                      |  static_cast<uint32_t>(datos[i + 3]);
        memoria(DR) = word;
    }
    for (; i < longitud; ++i)
        *reinterpret_cast<volatile uint8_t*>(base) = datos[i];

    return resultado();
}

uint32_t CRC::resultado() const
{
    return static_cast<uint32_t>(memoria(DR));
}
