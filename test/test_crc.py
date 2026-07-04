import binascii
import serial
import time

# Vector de prueba estándar CRC-32 (Ethernet, convención reflejada)
# Resultado esperado: 0xCBF43926
DATOS = b'123456789'
ESPERADO = binascii.crc32(DATOS) & 0xFFFFFFFF

# ---------------------------------------------------------------------------
# Código C++ correspondiente — copiar al archivo de producción
# ---------------------------------------------------------------------------
#
# #include "HAL/CRC.h"
# #include <cstdio>
#
# // En el setup / inicio del programa:
# CRC::Config config_crc;
# config_crc.rev_entrada  = CRC::RevEntrada::PorByte;   // Coincide con binascii.crc32()
# config_crc.rev_salida   = true;
# CRC crc(config_crc);
#
# const uint8_t datos[] = { '1','2','3','4','5','6','7','8','9' };
# uint32_t res = crc.calcular(datos, sizeof(datos));
#
# char buf[16];
# std::sprintf(buf, "%08lX\r\n", res);
# uart1 << buf;   // Reemplazar uart1 por la UART que uses
#
# // Resultado esperado en consola: 0xCBF43926
# ---------------------------------------------------------------------------

def calcular_esperado():
    resultado = binascii.crc32(DATOS) & 0xFFFFFFFF
    print(f"Datos:    {DATOS!r}")
    print(f"Esperado: 0x{resultado:08X}")
    return resultado


def verificar_con_mcu(port="COM5", baudrate=115200):
    esperado = calcular_esperado()

    try:
        with serial.Serial(port, baudrate, timeout=3) as ser:
            time.sleep(0.5)
            ser.reset_input_buffer()

            linea = ser.readline().decode(errors='replace').strip()
            print(f"MCU:      0x{linea}")

            recibido = int(linea, 16)
            if recibido == esperado:
                print("OK - CRC coincide")
                return True
            else:
                print(f"ERROR - Esperado 0x{esperado:08X}, recibido 0x{recibido:08X}")
                return False

    except serial.SerialException as e:
        print(f"Error serial: {e}")
        return False


if __name__ == "__main__":
    # Sin MCU conectado: sólo muestra el valor esperado
    calcular_esperado()

    # Con MCU conectado: descomentar la siguiente línea y ajustar el puerto
    # verificar_con_mcu(port="COM5", baudrate=115200)
