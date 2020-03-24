gcc hw4c_Jiang.c -o c -pthread
n=10
while ((n<=100000000))
	do
		./c $n
		let n*=10
	done
