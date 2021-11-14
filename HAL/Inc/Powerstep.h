//
// Created by migue on 13/11/2021.
//

#ifndef G070_POWERSTEP_H
#define G070_POWERSTEP_H

#include "SPI.h"
#include "GPIO_Port.h"

struct Powerstep {

  /** Las longitudes en bits de estos registros varían un *chingo* */
  enum class Registro : uint8_t {
    ABS_POS = 1,
    EL_POS,
    MARK,
    SPEED,
    ACC,
    DEC,
    MAX_SPEED,
    MIN_SPEED,
    KVAL_HOLD,
    KVAL_RUN,
    KVAL_ACC,
    KVAL_DEC,
    INT_SPEED,
    ST_SLP,
    FN_SLP_ACC, //0x0F
    FN_SLP_DEC, //0x10
    K_THERM,
    ADC_OUT,
    OCD_TH,
    STALL_TH,
    FS_SPD,
    STEP_MODE,
    ALARM_EN,
    GATECFG1,
    GATECFG2,
    CONFIG,
    STATUS, //alreves en datasheet -_-
  };

  Powerstep(const SPI& spi_arg, const GPIO::pin& SS_pin_arg);
  uint32_t GetParam(Registro r);
  void SetParam(Registro r, uint32_t valor);
  void Run(bool DIR, uint32_t speed);
  void StepClock(bool DIR);
  void Move(bool DIR, uint32_t steps);
  void GoTo(uint32_t abs_pos);
  void GoToDir(bool DIR, uint32_t abs_pos);
  void GoUntil(bool ACT, bool DIR, uint32_t speed);
  void ReleaseSW(bool ACT, bool DIR);
  void GoHome();
  void GoMark();
  void ResetPos();
  void ResetDevice();
  void SoftStop();
  void HardStop();
  void SoftHiZ();
  void HardHiZ();
  uint16_t GetStatus();

private:
  uint8_t recurrent_operation(const uint8_t m);

  const SPI& spi;
  const GPIO::pin& SS_pin;
};


#endif //G070_POWERSTEP_H
