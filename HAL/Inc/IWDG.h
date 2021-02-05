//
// Created by migue on 10/06/2020.
//

#ifndef G070_IWDG_H
#define G070_IWDG_H

/* Independent watch dog*/
#include <cstddef>

namespace mIWDG{
  constexpr size_t base = 0x40003000;
  constexpr size_t KR = (base); //limit 0x400033FF
  constexpr size_t PR = (base + 0x4);
  constexpr size_t RLR = (base + 0x8);
  constexpr size_t SR = (base + 0xC);
  constexpr size_t WINR = (base + 0x10);
  /*************************************/
  void enable_watchdog() { *reinterpret_cast<size_t*>(KR) = 0x0000CCCC;}
  void enable_regwrite() { *reinterpret_cast<size_t*>(KR) = 0x00005555;}
  void write_prescaler(const size_t val) { *reinterpret_cast<size_t*>(PR) = (val&0x7); }//values from 0 to 7
  void write_reload(const size_t val) { *reinterpret_cast<size_t*>(RLR) = (val&0xFFF); }
  void wait() {
    size_t status = 1;
    while(status != 0) { status = *reinterpret_cast<size_t*>(SR); };
  }
  void refresh() { *reinterpret_cast<size_t*>(KR) = 0x0000AAAA; }
  void set_and_go(const size_t prescaler, const size_t reload) {
    enable_watchdog();
    enable_regwrite();
    write_prescaler(prescaler);
    write_reload(reload);
    wait();
  }
};

#endif //G070_IWDG_H
