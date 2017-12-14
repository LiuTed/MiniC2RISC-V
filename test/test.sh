#!/bin/bash
make
rm tigger_result.txt riscv_result.txt

echo "running test: test"
for i in {1..30}
do
	echo "$i" | ./Tigger test.t >> tigger_result.txt
	echo "$i" | /opt/riscv-qemu/bin/qemu-riscv64 -L /opt/riscv/sysroot/ test >> riscv_result.txt
done

echo "running test: bubble"
for i in {1..100..10}
do
	str="$i"
	for j in $(seq 1 $i)
	do
		str=${str}" $RANDOM"
	done
	echo $str | ./Tigger bubble.t >> tigger_result.txt
	echo $str | /opt/riscv-qemu/bin/qemu-riscv64 -L /opt/riscv/sysroot/ ./bubble >> riscv_result.txt
done

echo "running test: manyreg"
/opt/riscv-qemu/bin/qemu-riscv64 -L /opt/riscv/sysroot/ ./manyreg >> riscv_result.txt
./Tigger manyreg.t >> tigger_result.txt

diff tigger_result.txt riscv_result.txt

