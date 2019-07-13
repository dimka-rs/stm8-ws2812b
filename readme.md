# stm8-ws2812b

**WS2812B timings**
- 0: 0.4 us high, 0.85 us low
- 1: 0.8 us high, 0.45 us low
- reset: 50+ us low
- half bit accuracy: ± 150 ns
- TH+TL = 1.25 us ± 600 ns

## using nops
**At 16 MHz**
- T0H - 5 nops
- T1H - 7 nops

## using timer

**At 8 MHz clock**
- T0H = 0.4  us: 3 x 0.125 = 0.375 (error: -0.025)
- T0L = 0.85 us: 7 x 0.125 = 0.875 (error: -0.025)
- T1H = 0.8  us: 6 x 0.125 = 0.75  (error: -0.050)
- T1L = 0.45 us: 4 x 0.125 = 0.5   (error: +0.050)

Seems acceptable, but loading next bit will take more than few tacts. Got to load byte, check bit, load CC value. Might have to use onle pulse mode, on CC load new value and restart timer.

