echostuff:
	echo "hello"


TigerS: TigerS.c
	gcc -g TigerS.c -o TigerS -lpthread

TigerC: TigerC.c
	gcc -g TigerC.c -o TigerC
