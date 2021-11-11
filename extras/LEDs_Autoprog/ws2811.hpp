#pragma once

#include <stdio.h>
#include <array>
#include <cmath>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"

#include "ws2811.pio.h"
#include <Arduino.h>

enum WS2811ColorMapping {
  RGB,
  GRB
};

union RGBLED
{
  uint32_t value;
  struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  } colors;
};

template<uint LEDS_TO_READ>
class LedResult
{
public:
	bool valid;
	std::array<RGBLED, LEDS_TO_READ> leds;
};


template <uint LEDS_TO_READ, WS2811ColorMapping MAPPING>
class WS2811Client {
private:
  PIO pio;
  uint offset;
  uint sm;
  pio_sm_config sm_conf;

  uint dma_ctrl_chan;
  uint dma_gather_chan;
  dma_channel_config dma_ctrl_conf;
  dma_channel_config dma_gather_conf;

  volatile uint32_t led_state[LEDS_TO_READ];
  const volatile uint32_t *led_state_address;

  inline void initGPIO() {
    pio_sm_set_consecutive_pindirs(pio, sm, DATA_IN_PIN, 1, GPIO_IN);
    pio_gpio_init(pio, DATA_IN_PIN);

#ifdef SIDESET_PIN
    pio_sm_set_consecutive_pindirs(pio, sm, SIDESET_PIN, 1, GPIO_OUT);
    pio_gpio_init(pio, SIDESET_PIN);
#endif

    pio_sm_set_consecutive_pindirs(pio, sm, DATA_OUT_PIN, 1, GPIO_OUT);
    pio_gpio_init(pio, DATA_OUT_PIN);
  }

  inline void initSMConfig() {
    sm_conf = ws2811_program_get_default_config(offset);

    sm_config_set_in_pins(&sm_conf, DATA_IN_PIN);
    sm_config_set_jmp_pin(&sm_conf, DATA_IN_PIN);
    sm_config_set_out_pins(&sm_conf, DATA_OUT_PIN, 1);
    sm_config_set_set_pins(&sm_conf, DATA_OUT_PIN, 1);
#ifdef SIDESET_PIN
    sm_config_set_sideset_pins(&sm_conf, SIDESET_PIN);
#endif

    sm_config_set_in_shift(&sm_conf, false, true, 24);  // shift left, auto push after 24 bit
    sm_config_set_out_shift(&sm_conf, false, false, 0); // shift left, no auto pull
    sm_config_set_fifo_join(&sm_conf, PIO_FIFO_JOIN_RX);

    sm_config_set_clkdiv(&sm_conf, 1);
  }

  inline void initDMA() {
    dma_ctrl_conf = dma_channel_get_default_config(dma_ctrl_chan);
    dma_gather_conf = dma_channel_get_default_config(dma_gather_chan);

    // CTRL
    {
      channel_config_set_transfer_data_size(&dma_ctrl_conf, DMA_SIZE_32);
      channel_config_set_read_increment(&dma_ctrl_conf, false);
      channel_config_set_write_increment(&dma_ctrl_conf, false);
      dma_channel_configure(
        dma_ctrl_chan,
        &dma_ctrl_conf,
        &dma_hw->ch[dma_gather_chan].al2_write_addr_trig, // write
        &led_state_address,                               // read
        1,
        false
      );
    }

    // GATHER
    {
      channel_config_set_transfer_data_size(&dma_gather_conf, DMA_SIZE_32);
      channel_config_set_read_increment(&dma_gather_conf, false);
      channel_config_set_write_increment(&dma_gather_conf, true);
      channel_config_set_dreq(&dma_gather_conf, pio_get_dreq(pio, sm, false));
      channel_config_set_chain_to(&dma_gather_conf, dma_ctrl_chan);
      dma_channel_configure(
        dma_gather_chan,
        &dma_gather_conf,
        &led_state[0], // write
        &pio->rxf[sm], // read
        LEDS_TO_READ,
        false
      );
    }

    dma_channel_start(dma_ctrl_chan);
  }

  inline void runSM() {
    pio_sm_clear_fifos(pio, sm);
    pio_sm_init(pio, sm, offset, &sm_conf);

    // init
    pio_sm_exec_wait_blocking(pio, sm, pio_encode_set(pio_y, 20));
    pio_sm_exec_wait_blocking(pio, sm, pio_encode_mov(pio_osr, pio_y));
    pio_sm_exec_wait_blocking(pio, sm, pio_encode_out(pio_null, 5));

    // wait for first reset pulse
    bool reset_finished = false;
    while (!reset_finished) {
      while (gpio_get(DATA_IN_PIN))
        ;

      const auto us = time_us_32();
      reset_finished = true;
      while (time_us_32() - us < 10) {
        if (gpio_get(DATA_IN_PIN)) {
          reset_finished = false;
          break;
        }
        tight_loop_contents();
      }
    }
    pio_sm_set_enabled(pio, sm, true);
  }

  inline const RGBLED ledStateToLED(const uint32_t val) const {
    switch (MAPPING) {
      case RGB:
        return {
          .colors = {
            .r = (uint8_t)((val >> 16) & 0xFF),
            .g = (uint8_t)((val >>  8) & 0xFF),
            .b = (uint8_t)((val >>  0) & 0xFF)
          }
        };
      case GRB:
        return {
          .colors = {
            .r = (uint8_t)((val >>  8) & 0xFF),
            .g = (uint8_t)((val >> 16) & 0xFF),
            .b = (uint8_t)((val >>  0) & 0xFF)
          }
        };
    }
  }

  static constexpr std::pair<uint, uint> getBitOffsets(const uint numLeds) {
    uint constPart = 0;
    uint shiftPart = 0;

    switch (numLeds) {
      case 1:
        constPart = 12;
        shiftPart =  1;
        break;
      case 2:
        constPart = 24;
        shiftPart =  1;
        break;
      case 3:
        constPart = 18;
        shiftPart =  2;
        break;
      case 4:
        constPart = 24;
        shiftPart =  2;
        break;
      case 5:
        constPart = 15;
        shiftPart =  3;
        break;
      case 6:
        constPart = 18;
        shiftPart =  3;
        break;
      case 7:
        constPart = 21;
        shiftPart =  3;
        break;
      case 8:
        constPart = 12;
        shiftPart =  4;
        break;
      case 9:
        constPart = 27;
        shiftPart =  3;
        break;
      case 10:
        constPart = 30;
        shiftPart =  3;
        break;
    }

    return std::make_pair(constPart, shiftPart);
  }

public:
  WS2811Client() : led_state_address(&led_state[0]) {
    if (pio_can_add_program(pio0, &ws2811_program)) {
      pio = pio0;
    } else if (pio_can_add_program(pio1, &ws2811_program)) {
      pio = pio1;
    } else {
      panic("Cannot start WS2811 client because both PIOs do not have enough space.");
    }
    offset = pio_add_program(pio, &ws2811_program);

    constexpr auto parts = WS2811Client::getBitOffsets(LEDS_TO_READ);
    static_assert(parts.first * pow(2, parts.second) == LEDS_TO_READ * 24);

    pio->instr_mem[offset + ws2811_offset_num_bits_const_1] = pio_encode_set(pio_x, parts.first);
    pio->instr_mem[offset + ws2811_offset_num_bits_const_2] = pio_encode_set(pio_y, parts.first);
    pio->instr_mem[offset + ws2811_offset_num_bits_shift_1] = pio_encode_in(pio_null, parts.second);
    pio->instr_mem[offset + ws2811_offset_num_bits_shift_2] = pio_encode_in(pio_null, parts.second);

    sm = pio_claim_unused_sm(pio, true);
    dma_gather_chan = dma_claim_unused_channel(true);
    dma_ctrl_chan = dma_claim_unused_channel(true);

    initGPIO();
    initSMConfig();
    initDMA();
    runSM();
  }

  ~WS2811Client() {
    pio_sm_set_enabled(pio, sm, false);

    channel_config_set_chain_to(&dma_gather_conf, dma_gather_chan);
    dma_channel_abort(dma_ctrl_chan);
    dma_channel_abort(dma_gather_chan);

    dma_channel_unclaim(dma_ctrl_chan);
    dma_channel_unclaim(dma_gather_chan);

    pio_remove_program(pio, &ws2811_program, offset);
    pio_sm_unclaim(pio, sm);

    // TODO: Deinit GPIO
  }

  const RGBLED getLED(uint idx) const {
    return ledStateToLED(led_state[idx]);
  }

	/*! \brief  Wait for a DMA channel transfer to complete
 *  \ingroup hardware_dma
 *
 * \param channel DMA channel
 */
static bool dma_channel_wait_for_finish(uint channel, uint64_t us) {
    uint64_t startTime = time_us_64();
		bool result;
    while ((time_us_64()-startTime)<us) {
				if (!dma_channel_is_busy(channel)) return true;
				tight_loop_contents();
    }
    return false;
}

const LedResult<LEDS_TO_READ> getLEDsAtomic(uint64_t us) {
		LedResult<LEDS_TO_READ> result;

    channel_config_set_chain_to(&dma_gather_conf, dma_gather_chan);
    if (dma_channel_wait_for_finish(dma_gather_chan, us))
		{
			for (uint i = 0; i < LEDS_TO_READ; i++) {
				result.leds[i] = ledStateToLED(led_state[i]);
			}
			channel_config_set_chain_to(&dma_gather_conf, dma_ctrl_chan);
			dma_channel_start(dma_ctrl_chan);
			result.valid = true;
			return result;
		}
		result.valid = false;
		return result;
  }

  const uint getNumLEDs() const {
    return LEDS_TO_READ;
  }
};
