import random

fout = open("in.txt", mode='w+')

size = 40

fout.write(str(size))
fout.write("\n")

for i in range(size):
    fout.write(str(random.randint(0,10000)))
    fout.write(" ")



