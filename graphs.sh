#!/bin/bash
./main --print_list > freq.dat
./main --print_data > supXconf.dat
gnuplot "freq.txt"
gnuplot "scatter.txt"
