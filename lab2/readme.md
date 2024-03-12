mkdir src
cd ./src
unzip zip
make
cd ../
./runit.sh ./outputdir ./src/sched
./gradeit.sh ./refout /outputdir


cd ./mysrc
make
cd ../
./runit.sh ./outputdir ./src/sched
./gradeit.sh ./refout /outputdir