#!/bin/sh

for yr in `seq 2000 2016`; do
	mkdir ${yr}
	cd ${yr}
	for mt in `seq 1 12`; do
		mkdir ${mt}
	done
	cd ..
done
