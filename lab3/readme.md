
# Words for Graders

My program is straightforward enough for graders. This Readme is in case that there is something wrong with my code and I need to debug. If graders cannot get perfect outputs during make or grading from my source code, this Readme may guide me and graders to find out compatibility issues.

Be sure run `module load gcc-9.2` first.

Unzip the `.zip` and place `makefile` and `main.cpp` in `<mycodesrc>`.

Run `make` to get `sched`

`./runit.sh <outputdir> ./<mycodesrc>/sched`

`./gradeit.sh <your_reference_dir> <outputdir_from_last_command_above>`

Then you will find out that how perfect my program is.



# Debug Method for myself

There should be `in*` and `runit.sh` and `gradeit.sh` and `rfile` and `/refprog` under current directory.

## optional on wsl

mkdir myprog

mkdir myout

mkdir refout

chmod +x ./runit.sh 

chmod +x ./gradeit.sh 

chmod +x ./refout/mmu

## mandatary on wsl

rm -rf ./myout/*

rm -rf ./refout/*

cd ./myprog

g++ -g main.cpp -o mmu

cd ../

./runit.sh ./myout ./myprog/mmu

./runit.sh ./refout ./refprog/mmu

./gradeit.sh ./refout ./myout


## optional on linserv

mkdir myprog

mkdir myout

mkdir refout

cd ./myprog

unzip ../qm2104.zip

make 

cd ../

chmod +x ./runit.sh 

chmod +x ./gradeit.sh 

chmod +x ./profsrc/sched

## mandatory  on linserv

rm -rf ./myout/*

rm -rf ./refout/*

cd ./mysrc

make

cd ../

./runit.sh ./myout ./mysrc/sched

./runit.sh ./refout ./profsrc/sched

./gradeit.sh ./refout ./myout
