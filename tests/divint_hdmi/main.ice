
// reset
import('verilog/reset_conditioner.v')

// clock
import('verilog/clk_wiz_v3_6.v')

// HDMI
append('verilog/serdes_n_to_1.v')
append('verilog/simple_dual_ram.v')
append('verilog/tmds_encoder.v')
append('verilog/async_fifo.v')
append('verilog/fifo_2x_reducer.v')
append('verilog/dvi_encoder.v')
import('verilog/hdmi_encoder.v')

// unsigned integer division
$include('../common/divint.ice')

// display
$include('display.ice')

algorithm main(
    output uint8 led,
    output uint1 spi_miso,
    input  uint1 spi_ss,
    input  uint1 spi_mosi,
    input  uint1 spi_sck,
    output uint4 spi_channel,
    input  uint1 avr_tx,
    output uint1 avr_rx,
    input  uint1 avr_rx_busy,
    // SDRAM
    output uint1 sdram_clk,
    output uint1 sdram_cle,
    output uint1 sdram_dqm,
    output uint1 sdram_cs,
    output uint1 sdram_we,
    output uint1 sdram_cas,
    output uint1 sdram_ras,
    output uint2 sdram_ba,
    output uint13 sdram_a,
    inout  uint8 sdram_dq,
    // HDMI
    output uint4 hdmi1_tmds,
    output uint4 hdmi1_tmdsb
) <@hdmi_clock,!stable_reset> {

uint1 hdmi_clock = 0;
uint1 stable_reset = 0;

// generate clock for HDMI
clk_wiz_v3_6 clk_gen (
  CLK_IN1  <: clock,
  CLK_OUT1 :> hdmi_clock
);

// reset conditionner
reset_conditioner rstcond (
  rcclk <: hdmi_clock,
  in  <: reset,
  out :> stable_reset
);

// bind to HDMI encoder
uint8  hdmi_red   = 0;
uint8  hdmi_green = 0;
uint8  hdmi_blue  = 0;
uint1  hdmi_pclk  = 0;
uint1  hdmi_active = 0;
uint11 hdmi_x = 0;
uint10 hdmi_y = 0;

hdmi_encoder hdmi (
  clk    <: hdmi_clock,
  rst    <: stable_reset,
  red    <: hdmi_red,
  green  <: hdmi_green,
  blue   <: hdmi_blue,
  pclk   :> hdmi_pclk,
  tmds   :> hdmi1_tmds,
  tmdsb  :> hdmi1_tmdsb,
  active :> hdmi_active,
  x      :> hdmi_x,
  y      :> hdmi_y
);

  text_display display<@hdmi_pclk>;

  uint21 counter = 0;
  uint8  myled   = 1;
  
  display.hdmi_x := hdmi_x;
  display.hdmi_y := hdmi_y;
  display.hdmi_active := hdmi_active;

  hdmi_red   := display.hdmi_red;
  hdmi_green := display.hdmi_green;
  hdmi_blue  := display.hdmi_blue;

  // ---------- unused pins

  spi_miso := 1bz;
  avr_rx := 1bz;
  spi_channel := 4bzzzz;

  // ---------- bindings

  // on-board LED output (I am alive!)
  led := myled;
  
  while (1) {

    counter = counter + 1;
    if (counter == 0) {
      myled = myled << 1;
      if (myled == 0) {
        myled = 1;
      }
    }

  }

}
