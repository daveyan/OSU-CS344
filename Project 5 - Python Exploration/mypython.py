#!/usr/bin/python

import sys
import random
import string

#empty string
file1char = ""
file2char = ""
file3char = ""

#generate a random lowercase char
for x in range(10):
    randomChar1 = random.choice(string.lowercase)
    randomChar2 = random.choice(string.lowercase)
    randomChar3 = random.choice(string.lowercase)
    #add it to each of the strings
    file1char += randomChar1
    file2char += randomChar2
    file3char += randomChar3

#eleventh char is a newline
file1char += "\n"
file2char += "\n"
file3char += "\n"

#open and write the string to the file
file = open('file1.txt','w')
file.write(file1char)
file.close()

file = open('file2.txt','w')
file.write(file2char)
file.close()

file = open('file3.txt','w')
file.write(file3char)
file.close()

#print for display
print("File 1: " + file1char)
print("File 2: " + file2char)
print("File 3: " + file3char)

#generate random int
num1 = random.randint(1,42)
num2 = random.randint(1,42)
#product
product = num1 * num2
#print
print("Integer 1: %d" % num1)
print("Integer 2: %d" % num2)
print("Product: %d" % product)





