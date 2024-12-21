`ifndef ICE40_SB_IO
`define ICE40_SB_IO

module sb_io(
  input        clock,
  input        out,
  output       pin
  );

  SB_IO #(
    .PIN_TYPE(6'b0101_01)
    //                ^^ ignored (input)
    //           ^^^^ registered output
  ) sbio (
      .PACKAGE_PIN(pin),
      .D_OUT_0(out),
      .OUTPUT_CLK(clock)
  );

endmodule

`endif

// http://www.latticesemi.com/~/media/LatticeSemi/Documents/TechnicalBriefs/SBTICETechnologyLibrary201504.pdf
