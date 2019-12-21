#!/bin/bash

mkdir client_storage
cd client_storage
touch upload{1..100}.txt

for n in {1..100}
do 
	echo "This file is at the client to be uploaded in the server" > upload${n}.txt
done

#I run this bash shell script in the Desktop path. Please do the same since the C program has been written considering the folder location as Desktop
#Therefore it creates a folder/directory named client_storage on the Desktop. Also, 100 files are created with same content in all of them.
