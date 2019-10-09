#! /usr/bin/env sh

if [[ -z $2 ]]
then
    echo "Usage: ./run-simulation.sh SIM_EXE_PATH CONFIG_DIR"
    exit 1
fi

for CONFIG in $2/*.siminput
do
    $1 --input $CONFIG
    echo
    $1 --input $CONFIG --vcs
    echo
done
