#pragma once

#include "helpers.h"
#include "RCC.h"

struct CRC
{
    enum class TamanioPolinomio : uint8_t {
        P32 = 0,
        P16 = 1,
        P8  = 2,
        P7  = 3
    };
    enum class RevEntrada : uint8_t {
        SinInversion   = 0,
        PorByte        = 1,
        PorMediaPalabra = 2,
        PorPalabra     = 3
    };

    struct Config {
        TamanioPolinomio tamanio_polinomio { TamanioPolinomio::P32 };
        RevEntrada rev_entrada             { RevEntrada::SinInversion };
        bool rev_salida                    { false };
        uint32_t valor_inicial             { 0xFFFFFFFF };
        uint32_t polinomio                 { 0x04C11DB7 };
    };

    const size_t base { 0x40023000 };
    registro DR, IDR, CR_reg, INIT, POL;

    CRC();
    explicit CRC(const Config&);

    void reset();
    uint32_t calcular(uint32_t dato);
    uint32_t calcular(uint16_t dato);
    uint32_t calcular(uint8_t dato);
    uint32_t calcular(const uint8_t* datos, size_t longitud);
    uint32_t resultado() const;
};
