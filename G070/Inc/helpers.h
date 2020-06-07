//
// Created by migue on 05/06/2020.
//

#ifndef G070_HELPERS_H
#define G070_HELPERS_H

#include <cstddef>
#include <cstdint>

struct registro
{
  registro(const size_t addr_) :
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
  bitfield(const uint8_t size, const uint8_t offset):
      mask((1U << size) - 1U), offset(offset) {}
  /* Does not write the register. Returns a number that you can OR with other bitfields to then write the register. */
  const size_t operator()(const size_t val) const { return (val&mask) << offset; } //(slave_addr&0x3FF) << SADDR
  const size_t mask;
  const uint8_t offset;
};


#endif //G070_HELPERS_H
