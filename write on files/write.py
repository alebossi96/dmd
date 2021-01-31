from hadamardOrdering import cake_cutting
for i in range(4,10):
	n= 2**i
	H = (cake_cutting(n)+1)/2
	f = open(str(n)+".txt",'w')
	for j in range(n):
		for k in range(n):
			f.write(str(int(H[j][k]))+" ")
		f.write("\n")
	f.close();
