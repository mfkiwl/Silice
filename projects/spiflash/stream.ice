$$if ICARUS then
// download W25Q128JVxIM from winbond Verilog models
append('W25Q128JVxIM/W25Q128JVxIM.v')
import('simul_spiflash.v')
$$end

$$uart_in_clock_freq_mhz = 12
$include('../common/uart.ice')

$include('spiflash.ice')

circuitry wait20() // waits exactly 20 cycles
{
  uint5 n = 0; while (n != 18) { n = n + 1; }
}

algorithm main(
  output uint8 leds,
$$if QSPIFLASH then
  output uint1 sf_clk,
  output uint1 sf_csn,
  inout  uint1 sf_io0,
  inout  uint1 sf_io1,
  inout  uint1 sf_io2,
  inout  uint1 sf_io3,
$$end
$$if UART then
  output uint1 uart_tx,
  input  uint1 uart_rx,
$$end
  )
{

$$if SIMULATION then
  uint1 sf_csn(1);
  uint1 sf_clk(0);
  uint1 sf_io0(0);
  uint1 sf_io1(0);
  uint1 sf_io2(0);
  uint1 sf_io3(0);
$$if ICARUS then
  simul_spiflash simu(
    CSn <:  sf_csn,
    CLK <:  sf_clk,
    IO0 <:> sf_io0,
    IO1 <:> sf_io1,
    IO2 <:> sf_io2,
    IO3 <:> sf_io3,
  );
$$end
  uint32 cycle(0);
$$end

  bram uint8 data[256] = uninitialized;

  uart_out uo;
$$if UART then
  uart_sender usend(
    io      <:> uo,
    uart_tx :>  uart_tx
  );
$$end

spiflash_rom sf_rom(
    sf_clk :>  sf_clk,
    sf_csn :>  sf_csn,
    sf_io0 <:> sf_io0,
    sf_io1 <:> sf_io1,
    sf_io2 <:> sf_io2,
    sf_io3 <:> sf_io3,
  );

  always {
    uo.data_in_ready = 0;
    sf_rom.in_ready  = 0;
$$if SIMULATION then
    cycle = cycle + 1;
    if (cycle == 1500) {
      __finish();
    }
$$end
  }

  while (sf_rom.busy) { }

  // read some
  data.wenable            = 1;
  while (data.addr != 64) {
    sf_rom.in_ready = 1;
    sf_rom.addr     = data.addr;
    ()              = wait20();
    __display("read %x",sf_rom.rdata);
    data.wdata      = sf_rom.rdata;
    data.addr       = data.addr + 1;
  }

  // output to UART
  data.wenable = 0;
  data.addr    = 1;
  while (data.addr != 64) {
    uo.data_in       = data.rdata;
    uo.data_in_ready = 1;
    data.addr        = data.addr + 1;
    while (uo.busy) { }
  }

}
