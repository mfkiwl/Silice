bitfield Node {
  uint16 right,
  uint16 left
}

algorithm main()
{
  uint32 data = Node(left=16hffff,right=16h1234);

  uint16 l = 33;
 
  Node(data).left = 33;
 }
 