void pedazos()
{
    general_timer t16(GeneralTimer::TIM16, general_timer::Mode::Periodic, 0x8, 0x100); //cada tick es de 1ms, con HSI de 16 MHz
    t16.enable_output_compare(0x1);
    t16.start();
    
    basic_timer t6(BasicTimer::TIM6, basic_timer::Mode::Periodic, 0x1800, 0x800);
    t6.enable_interrupt(led_callback);
    t6.start();

    general_timer t17(GeneralTimer::TIM17, general_timer::Mode::OnePulseMode, 0x2000, 0x800);
    t17.enable_interrupt(callback1);
}