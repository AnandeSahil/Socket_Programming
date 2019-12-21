x=1
while [ $x -le 100 ]
do
  ./TigerC <<< "tconnect 127.0.0.1 user pass
tget down$x.txt
tput upload$x.txt
exit" &
  x=$(( $x + 1 ))
done
