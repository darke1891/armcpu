name=$1
[ -z $name ] && name=memtrans2

../../utils/asm2bin.sh $name.s $name.bin /tmp/$name.s
./genrom $name.bin $name.v
