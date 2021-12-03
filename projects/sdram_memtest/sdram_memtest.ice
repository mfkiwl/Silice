// SL 2020
//
// A simple test for SDRAM controllers (ULX3S / simulation)
//
// -------------------------
// MIT license, see LICENSE_MIT in Silice repo root
// https://github.com/sylefeb/Silice
// @sylefeb 2020

$$TEST_r512w64            = true
$$TEST_r128w8             = false
$$TEST_r16w16             = false

$$TEST_with_autoprecharge = true

$include('../common/sdram_interfaces.ice')

$$if TEST_r128w8 then
$$  if TEST_with_autoprecharge then
$$      print('TESTING sdram_controller_autoprecharge_r128_w8')
$include('../common/sdram_controller_autoprecharge_r128_w8.ice')
$$  else
$$    print('TESTING sdram_controller_r128_w8')
$include('../common/sdram_controller_r128_w8.ice')
$$  end
$$end

$$if TEST_r16w16 then
$$  print('TESTING sdram_controller_autoprecharge_r16_w16')
$include('../common/sdram_controller_autoprecharge_r16_w16.ice')
$$end

$$if TEST_r512w64 then
$$  print('TESTING sdram_controller_autoprecharge_pipelined_r512_w64')
$include('../common/sdram_controller_autoprecharge_pipelined_r512_w64.ice')
$$end

$include('../common/sdram_utils.ice')

$$if ICARUS then
// SDRAM simulator
append('../common/mt48lc16m16a2.v')
import('../common/simul_sdram.v')
$$end

$$if ULX3S then
// Clock
import('../common/plls/ulx3s_50_25_100_100ph180.v')
$$end

$$if SIMULATION then
$$  TEST_SIZE = 1<<10
$$else
$$  TEST_SIZE = 1<<24
$$end

$include('../common/clean_reset.ice')

// -------------------------

algorithm main(
  output uint$NUM_LEDS$ leds,
$$if not ICARUS then
  // SDRAM
  output! uint1  sdram_cle,
  output! uint2  sdram_dqm,
  output! uint1  sdram_cs,
  output! uint1  sdram_we,
  output! uint1  sdram_cas,
  output! uint1  sdram_ras,
  output! uint2  sdram_ba,
  output! uint13 sdram_a,
$$if VERILATOR then
  output! uint1  sdram_clock,
  input   uint16 sdram_dq_i,
  output! uint16 sdram_dq_o,
  output! uint1  sdram_dq_en,
  // VGA (to be compiled with sdram_vga framework)
  output! uint1  video_clock,
  output! uint4  video_r,
  output! uint4  video_g,
  output! uint4  video_b,
  output! uint1  video_hs,
  output! uint1  video_vs
$$else
  output uint1  sdram_clk,  // sdram chip clock != internal sdram_clock
  inout  uint16 sdram_dq,
$$end
$$end
)
$$if ULX3S then
<@sdram_clock,!rst>
$$end
{

$$if not ICARUS then
uint1 rst = uninitialized;
clean_reset rstcond<@sdram_clock,!reset> (
  out   :> rst
);
$$end

uint16 iter = 0;

// --- SDRAM

$$if ICARUS then

uint1  sdram_cle   = 0;
uint2  sdram_dqm   = 0;
uint1  sdram_cs    = 0;
uint1  sdram_we    = 0;
uint1  sdram_cas   = 0;
uint1  sdram_ras   = 0;
uint2  sdram_ba    = 0;
uint13 sdram_a     = 0;
uint16 sdram_dq    = 0;

simul_sdram simul(
  sdram_clk <:  clock,
  sdram_cle <:  sdram_cle,
  sdram_dqm <:  sdram_dqm,
  sdram_cs  <:  sdram_cs,
  sdram_we  <:  sdram_we,
  sdram_cas <:  sdram_cas,
  sdram_ras <:  sdram_ras,
  sdram_ba  <:  sdram_ba,
  sdram_a   <:  sdram_a,
  sdram_dq  <:> sdram_dq
);

$$end

  // SDRAM interface
$$if TEST_r128w8 then
  sdram_r128w8_io sio;
$$end
$$if TEST_r16w16 then
  sdram_r16w16_io sio;
$$end
$$if TEST_r512w64 then
  sdram_r512w64_io sio;
$$end
  // algorithm
$$if TEST_r128w8 then
$$  if TEST_with_autoprecharge then
  sdram_controller_autoprecharge_r128_w8 sdram(
$$  else
  sdram_controller_r128_w8 sdram(
$$  end
$$end
$$if TEST_r16w16 then
  sdram_controller_autoprecharge_r16_w16 sdram(
$$end
$$if TEST_r512w64 then
  sdram_controller_autoprecharge_pipelined_r512_w64 sdram(
$$end
    sd        <:> sio,
    sdram_cle :>  sdram_cle,
    sdram_dqm :>  sdram_dqm,
    sdram_cs  :>  sdram_cs,
    sdram_we  :>  sdram_we,
    sdram_cas :>  sdram_cas,
    sdram_ras :>  sdram_ras,
    sdram_ba  :>  sdram_ba,
    sdram_a   :>  sdram_a,
  $$if VERILATOR then
    dq_i      <:  sdram_dq_i,
    dq_o      :>  sdram_dq_o,
    dq_en     :>  sdram_dq_en,
  $$else
    sdram_dq  <:> sdram_dq
  $$end
  );

  uint16               cycle(0);
  uint27               count = uninitialized;
  uint2                pass  = 0;

$$if VERILATOR then
  // sdram clock for verilator simulation
  sdram_clock := clock;
$$end

$$if ULX3S then
  // --- clock
  uint1 video_clock   = 0;
  uint1 sdram_clock   = 0;
  uint1 pll_lock      = 0;
  uint1 compute_clock = 0;
  uint1 compute_reset = 0;
  $$print('ULX3S at 50 MHz compute clock, 100 MHz SDRAM')
  pll_50_25_100_100ph180 clk_gen(
    clkin    <: clock,
    clkout0  :> compute_clock,
    clkout1  :> video_clock,
    clkout2  :> sdram_clock, // controller
    clkout3  :> sdram_clk,   // chip
    locked   :> pll_lock
  );
$$end

  // maintain low (pulses when ready, see below)
  sio.in_valid := 0;

/*
always_before {
  if (sio.done) {
    __display("[cycle %d] DONE",cycle);
  }
}

always_after {
  if (sio.in_valid) {
    __display("[cycle %d] REQ",cycle);
  }
  cycle = cycle + 1;
}
*/

$$if false then

  iter = 0;
  while (iter < 32) { // init
  iter         = iter + 1;
  }

  iter = 0;
  while (iter < 1) {
    sio.rw       = 1;
    sio.addr     = iter;
    sio.data_in  = 64h8877665544332211;
    sio.wmask    = 8b10101010;
    sio.in_valid = 1;
    while (!sio.done) { }
    iter         = iter + 8;
  }

  sio.rw       = 0;
  sio.addr     = 0;
  sio.data_in  = 0;
  sio.in_valid = 1;
  while (!sio.done) { }
  __display("sio.data_out = %h",sio.data_out);
  leds = sio.data_out[56,8];
$$end

$$if true then
  while (pass < 2) {
    sio.rw = ~pass[0,1];
    leds   = 8b01000100;
    count  = 0;
    while (count < $TEST_SIZE$) {
      // write to sdram
  $$if TEST_r128w8 or TEST_r16w16 then
      sio.data_in    = count[0,8];
  $$else
      sio.data_in    = 64h1122aabbccddeeff ^ count ^ (count<<32);
  $$end
      sio.addr       = count;
      sio.in_valid   = 1; // go ahead!
      while (!sio.done) { }
      if (~pass[0,1]) {
        leds = count[16,8];
        $$if SIMULATION then
        if (count < 128 || count > $TEST_SIZE-128$) {
          __display("write [%x] = %x",count,sio.data_in);
        }
        $$end
      } else {
$$if TEST_r128w8 or TEST_r16w16 then
        if (sio.data_out[0,8] != count[0,8]) {
          leds = 8b00010001;
          __display("ERROR AT %h",count);
        }
$$else
        if (sio.data_out[0,64] != (64h1122aabbccddeeff ^ count ^ (count<<32))) {
          leds = 8b00010001;
          __display("ERROR AT %h",count);
        }
$$end
        $$if SIMULATION then
        if (count < 128 || count >= $TEST_SIZE-128$) {
          __display("read  [%x] = %x",count,sio.data_out);
        }
        $$end
      }
  $$if TEST_r128w8 then
      count = count + (pass ? 16 : 1);
  $$end
  $$if TEST_r16w16 then
      count = count + 2;
  $$end
  $$if TEST_r512w64 then
      count = count + (pass ? 64 : 8);
  $$end
    }
    pass = pass + 1;
  }
$$end
}
