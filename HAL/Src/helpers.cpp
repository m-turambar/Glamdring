//
// Created by migue on 05/06/2020.
//

#include "helpers.h"

/** Nota
 * Estas funciones son nuestra principal herramienta para leer y escribir de memoria */

constexpr size_t operator&(const size_t addr, const flag f)
{
  return addr & f.mask;
}

volatile size_t& memoria(const size_t loc)
{
  return *reinterpret_cast<size_t*>(loc);
}

volatile uint16_t & memoria16(const size_t loc)
{
  return *reinterpret_cast<uint16_t *>(loc);
}

volatile size_t& memoria(const registro reg)
{
  return *reinterpret_cast<size_t*>(reg.addr);
}

volatile uint16_t& memoria16(const reg16 reg)
{
  return *reinterpret_cast<uint16_t *>(reg.addr);
}

void registro::set(flag f) const
{
  memoria(addr) |= f.mask;
}

bool registro::is_set(flag f) const
{
  return (memoria(addr) & f) != 0;
}

void registro::reset(flag f) const
{
  memoria(addr) &= !f;
}

bool registro::is_reset(flag f) const
{
  return (memoria(addr) & f) == 0;
}

void registro::write(const bitfield bf) const
{
  size_t tmp = memoria(addr);
  bf.overwrite(tmp);
  memoria(addr) = tmp;
}

void registro::reset(const bitfield bf) const
{
  size_t tmp = memoria(addr);
  bf.clear(tmp);
  memoria(addr) = tmp;
}

size_t registro::read(const bitfield bf) const
{
  return bf.mask & memoria(addr);
}

size_t registro::read_grounded(const bitfield bf) const
{
  return (bf.mask & memoria(addr)) >> bf.offset;
}

/******************************************************/
/******************************************************/
/******************************************************/

void reg16::set(const flag f) const
{
  memoria16(addr) |= static_cast<uint16_t>(f.mask);
}

void reg16::reset(const flag f) const
{
  memoria16(addr) &= !static_cast<uint16_t>(f.mask);
}

bool reg16::is_set(const flag f) const
{
  return (memoria16(addr) & static_cast<uint16_t>(f.mask)) != 0;
}

bool reg16::is_reset(const flag f) const
{
  return (memoria16(addr) & static_cast<uint16_t>(f.mask)) == 0;
}

void reg16::write(const bitfield bf) const
{
  size_t tmp = memoria16(addr);
  bf.overwrite(tmp);
  memoria16(addr) = static_cast<uint16_t>(tmp);
}

void reg16::reset(const bitfield bf) const
{
  size_t tmp = memoria16(addr);
  bf.clear(tmp);
  memoria16(addr) = static_cast<uint16_t>(tmp);
}

uint16_t reg16::read(const bitfield bf) const
{
  return bf.mask & memoria16(addr);
}

uint16_t reg16::read_grounded(const bitfield bf) const
{
  return (bf.mask & memoria16(addr)) >> bf.offset;
}
