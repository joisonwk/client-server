#!/bin/sh
rm -rf test-bin client-bin server-bin client-test *.conf
cd ../test 
make clean
make 
cp ./test-bin ../bin


cd ../clt_src
make clean
make client-bin
cp ./client-bin ../bin


cd ../clt_src 
make clean 
make client-test 
cp ./client-test ../bin

cd ../srv_src 
make clean 
make 
cp ./server-bin ../bin

cd ../srv_src 
cp *.conf ../bin

cd ../bin



