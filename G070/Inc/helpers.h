//
// Created by migue on 05/06/2020.
//

#ifndef G070_HELPERS_H
#define G070_HELPERS_H

#include <cstddef>
#include <cstdint>

struct registro
{
  constexpr registro(const size_t addr_) :
      addr(addr_) {}
  const size_t addr;
};

/* el namespace anónimo le da storage estático */
volatile size_t& memoria(const size_t loc);
volatile size_t& memoria(const registro reg);

/* abstracción para interactuar con los campos de bits de un registro */
struct bitfield
{
  /* sz is the number of bits this bitfield has, offset is number of bits offset from register base address */
  constexpr bitfield(const uint8_t size, const uint8_t offset):
      mask((1U << size) - 1U), offset(offset) {}
  /* Does not write the register. Returns a number that you can OR with other bitfields to then write the register. */
  constexpr size_t operator ()(const size_t val) const { return (val&mask) << offset; } //(slave_addr&0x3FF) << SADDR
  /** Para operaciones de resetear bitfields en un registro*/
  constexpr size_t operator !() const { return ~(mask << offset); }
  /** Regresa solo los bits del bitfield de ese registro
   * reg: 11001010 10110000 11100010 00001111
   * bf:  00000000 11100000 00000000 00000000
   * ret: 00000000 10100000 00000000 00000000*/
  const size_t operator ()(const registro reg) const { return ((mask << offset)&(memoria(reg))); }
  const size_t mask;
  const uint8_t offset;
};


#endif //G070_HELPERS_H
