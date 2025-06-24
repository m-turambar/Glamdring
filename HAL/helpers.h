//
// Created by migue on 05/06/2020.
//

#ifndef GLAMDRING_HELPERS_H
#define GLAMDRING_HELPERS_H

#include <cstddef>
#include <cstdint>

/** No te concentres en hacer las instancias de estas clases const o constexpr.
 * Al parecer no causa ninguna diferencia. Sin embargo, usar flag sobre bitfield sí reduce un poco la flash que usas.
 * */

struct bitfield;

/** Es un bitfield de un solo bit */
struct flag
{
  constexpr explicit flag(const uint8_t offset) :
    mask(1 << offset) {}
  constexpr size_t operator !() const { return ~(mask); }
  const size_t mask;
};

/** Abstracción para operar registros de 32 bits. */
struct registro
{
  constexpr explicit registro(const size_t addr) :
  addr(addr) {}

  void set(flag f) const;
  void reset(flag f) const;
  bool is_set(flag f) const;
  bool is_reset(flag f) const;
  /** Escribe ese bitfield dejando el resto del registro intacto. No olvides poner un valor en el bitfield. */
  void write(const bitfield& bf) const;

  /** Clearea en memoria los bits del registro que corresponden a este bitfield. */
  void reset(const bitfield& bf) const;
  /** Regresa el valor del bitfield con todo y offset. */
  size_t read(const bitfield& bf) const;
  /** Regresa el valor del bitfield sin su offset. */
  size_t read_grounded(const bitfield& bf) const;

  const size_t addr;
};

/** Abstracción para operar registros de 16 bits. */
struct reg16
{
  constexpr explicit reg16(const size_t addr_) :
      addr(addr_) {}

  void set(flag f) const;
  void reset(flag f) const;
  bool is_set(flag f) const;
  bool is_reset(flag f) const;
  /** Escribe ese bitfield dejando el resto del registro intacto. No olvides poner un valor en el bitfield. */
  void write(const bitfield& bf) const;

  /** Clearea en memoria los bits del registro que corresponden a este bitfield. */
  void reset(const bitfield& bf) const;
  /** Regresa el valor del bitfield con todo y offset. */
  uint16_t read(const bitfield& bf) const;
  /** Regresa el valor del bitfield sin su offset. */
  uint16_t read_grounded(const bitfield& bf) const;

  const size_t addr; //esto sí sigue siendo size_t porque necesitamos 32 bits para el espacio de direcciones
};

/* el namespace anónimo le da storage estático */
volatile size_t& memoria(registro reg);
volatile size_t& memoria(size_t loc);
volatile uint16_t& memoria16(reg16 reg);
volatile uint16_t& memoria16(size_t loc);

/** Abstracción para interactuar con los campos de bits de un registro.
 * Es preferible que el usuario use las funciones miembro de registro,
 * con bitfields o flags como argumentos. */
struct bitfield
{
  /** size es el número de bits del campo.
   * offset es el desplazamiento a la izquierda que tiene el bitfield desde el lsb.
   * val_arg es el valor opcional(recomendado) que queremos que el bf tenga.
   *
   * Ejemplo:
   * size = 3
   * offset = 22
   * val_arg = 2
   *
   * val_arg:   00000000 00000000 00000000 00000010
   * mask:      00000000 11100000 00000000 00000000
   * value:     00000000 01000000 00000000 00000000
   * offset:    22
   * */
  constexpr bitfield(const uint8_t size, const uint8_t offset, const size_t val_arg=0):
      mask(((1U << size) - 1U) << offset),
      offset(offset),
      value((val_arg << offset) & mask) {}

  /** Regresa val shifteado a la izquierda el offset del bitfield, y enmascarado. */
  constexpr size_t operator ()(const size_t val) const { return (val << offset) & mask; }

  /** Para operaciones de resetear bitfields en un registro. */
  constexpr size_t operator !() const { return ~(mask); }

  /** Lectura: Regresa el valor del bitfield en el registro leido
   * reg: 11001010 10110000 11100010 00001111
   * bf:  00000000 11100000 00000000 00000000
   * ret: 00000000 10100000 00000000 00000000 */
  size_t operator ()(const registro reg) const { return mask & memoria(reg); }

  void clear(size_t& clearee) const { clearee = clearee & (~mask); }
  void write(size_t& writee) const { writee = writee | value; }
  void overwrite(size_t& word) const { clear(word); write(word); }

  const size_t mask;
  const uint8_t offset;
  const size_t value;
};


#endif //GLAMDRING_HELPERS_H
