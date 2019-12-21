#!/bin/bash


mkdir server_storage
cd server_storage
touch down{1..100}.txt

for n in {1..100}
do 
	echo "This file is at the server to be downloaded by client" > down${n}.txt
done

#I run this bash shell script in the Desktop path. Please do the same since the C program has been written considering the folder location as Desktop
#Therefore it creates a folder/directory named server_storage on the Desktop. Also, 100 files are created with same content in all of them.
