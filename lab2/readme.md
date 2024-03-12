
# Words for Graders

My program is straightforward enough for graders. This Readme is in case that there is something wrong with my code and I need to debug. If graders cannot get perfect outputs during make or grading from my source code, this Readme may guide me and graders to find out compatibility issues.

Be sure run `module load gcc-9.2` first.

Unzip the `.zip` and place `makefile` and `main.cpp` in `<mycodesrc>`.

Run `make` to get `sched`

`./runit.sh <outputdir> ./<mycodesrc>/sched`

`./gradeit.sh <your_reference_dir> <outputdir_from_last_command_above>`

Then you will find out that how perfect my program is.



# Debug Method for myself

There should be `input*` and `runit.sh` and `gradeit.sh` and `rfile` and `/profsrc` under current directory.

## optional

mkdir mysrc

mkdir myout

mkdir refout

cd ./mysrc

unzip ../qm2104.zip

make 

cd ../

chmod +x ./runit.sh 

chmod +x ./gradeit.sh 

chmod +x ./profsrc/sched

## mandatory

rm -rf ./myout/*

rm -rf ./refout/*

cd ./mysrc

make

cd ../

./runit.sh ./myout ./mysrc/sched

./runit.sh ./refout ./profsrc/sched

./gradeit.sh ./refout ./myout
