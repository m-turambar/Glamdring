//
// Created by migue on 05/06/2020.
//

#include "helpers.h"

volatile size_t& memoria(const size_t loc)
{
  return *reinterpret_cast<size_t*>(loc);
}
