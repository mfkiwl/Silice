// SL 2020-10-25
// https://github.com/sylefeb/Silice
// MIT license, see LICENSE_MIT in Silice repo root

$$if not CLEAN_RESET_ICE then
$$  CLEAN_RESET_ICE = 1

algorithm clean_reset(
  output uint1 out(1)
) <autorun> {
  uint4  trigger = 15;
  always {
    out     = trigger[0,1];
    trigger = trigger >> 1;
  }
}

$$end
