int spi_loopback_test(SPI& spi)
{
  int n_errors = 0;
  for(uint8_t i = 0; i<255; ++i)
  {
    spi.escribir(i);
    uint8_t msg = spi.leer();
    if(msg != i)
      n_errors++;
  }
  return n_errors;
}