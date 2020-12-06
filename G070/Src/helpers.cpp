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

volatile size_t& memoria(const registro reg)
{
  return *reinterpret_cast<size_t*>(reg.addr);
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
  size_t tmp = memoria(*this);
  bf.overwrite(tmp);
  memoria(*this) = tmp;
}

void registro::reset(const bitfield bf) const
{
  size_t tmp = memoria(*this);
  bf.clear(tmp);
  memoria(*this) = tmp;
}

size_t registro::read(const bitfield bf) const
{
  return bf.mask & memoria(*this);
}

size_t registro::read_grounded(const bitfield bf) const
{
  return (bf.mask & memoria(*this)) >> bf.offset;
}

