
algorithm alg(input uint8 num,output uint8 ret)
{
  ret = num;
}

algorithm main(
  output int8 led 
)
{
  uint8 val = 231;

  alg alg0;

  (led) <- alg0 <- (val);
}

