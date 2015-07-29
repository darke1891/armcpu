name=$1

size=`wc -c $1 | awk '{print $1}'`

echo "file size : $size"

tmp_file=/tmp/flash_check

./controller.py flash erase 0 $size
./controller.py flash write 0 $name
./controller.py flash read 0 $size $tmp_file

diff $name $tmp_file
