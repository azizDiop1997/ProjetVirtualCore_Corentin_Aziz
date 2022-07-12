# coding=utf8

import sys
import struct
import os



dict_opcode = {"AND":0,"ORR":1,"EOR":2,"ADD":3,"ADC":4,"CMP":5,"SUB":6,"SBC":7,"MOV":8,"LSH":9,"RSH":10}

dict_bcc = {"B":8,"BEQ":9,"BNE":10,"BLE":11,"BGE":12,"BL":13,"BG":14}
		

def instrutionType(instr) :
	if( instr[0] == 'B') :
		return 0
	else :
		return 1

def encryptBranchInstr(instr) :
	instr = instr.split(' ')
	bcc = dict_bcc[instr[0]]
	offset = instr[1].rstrip("\n")

	# case negative offset
	if( offset[0] == "-") :
		offset = offset[1:]
		offset = int(offset,10)
		binary = bcc << 28 | 1 << 27 | offset << 0
		print(bin(binary))
		return struct.pack('>I',binary)

	# case positive offset
	else :
		offset = int(offset,10)
		binary = bcc << 28 | offset << 0
		print(bin(binary))
		return struct.pack('>I',binary)

def cmpInstr(instr, opcode) :
	ope1 = int(instr[1][1],10)
	#compare with a register
	if ( instr[2][0] == 'r') :
		ope2 = int(instr[2][1],16)
		binary = 0 << 28 | 0 << 25 | 0 << 24 | opcode << 20 | ope1 << 16 | ope2 << 12 
		print(bin(binary))
		print( "ope1 = " + str(ope1) + " & ope2 = " + str(ope2) )
		return struct.pack('>I',binary)
	#compare with a value
	else :
		#format hex
		if( instr[2][0:2] == "0x" ) :
			iv = int(instr[2][2:],16)
			binary = 0 << 28 | 0 << 25 | 1 << 24 | opcode << 20 | ope1 << 16 | iv
			print(bin(binary))
			print( "ope1 = " + str(ope1) + " & iv = " + str(iv))
			print(bin(binary))
			return struct.pack('>I',binary)
		#format decimal
		else :
			iv = int(instr[2],10)
			binary = 0 << 28 | 0 << 25 | 1 << 24 | opcode << 20 | ope1 << 16 | iv
			print(bin(binary))
			print( "ope1 = " + str(ope1) + " & iv = " + str(iv))
			return struct.pack('>I',binary)

def movInstr(instr, opcode) :
	dest = int(instr[1][1], 10)
	if ( instr[2][0] == 'r') :
		ope2 = int(instr[2][1],16)
		binary = 0 << 28 | 0 << 25 | 0 << 24 | opcode << 20 | ope2 << 12 | dest << 8
		print(bin(binary))
		print( "dest = " + str(dest) + " & ope2 = " + str(ope2))
		return struct.pack('>I',binary)
	else :
		#format hex
		if( instr[2][0:2] == "0x" ) :
			iv = int(instr[2][2:],16)
			binary = 0 << 28 | 0 << 25 | 1 << 24 | opcode << 20 | dest << 8 | iv
			print(bin(binary))
			print( "dest = " + str(dest) + " & iv = " + str(iv))
			return struct.pack('>I',binary)
		#format decimal
		else :
			iv = int(instr[2],10)
			binary = 0 << 28 | 0 << 25 | 1 << 24 | opcode << 20 | dest << 8 | iv
			print(bin(binary))
			print( "dest = " + str(dest) + " & iv = " + str(iv))
			return struct.pack('>I',binary)

def otherOpeInstr(instr, opcode) :
	dest = int(instr[1][1], 10)
	ope1 = int(instr[2][1], 10)
	if ( instr[3][0] == 'r') :
		ope2 = int(instr[3][1],16)
		binary = 0 << 28 | 0 << 25 | 0 << 24 | opcode << 20 | ope1 << 16 | ope2 << 12 | dest << 8
		print(bin(binary))
		print( "opcode = " + str(opcode) + " & dest = " + str(dest) + " & ope1 = " + str(ope1) + " & ope2 = " + str(ope2))
		return struct.pack('>I',binary)
	else :
		if( instr[3][0:2] == "0x" ) :
			iv = int(instr[3][2:],16)
			binary = 0 << 28 | 0 << 25 | 1 << 24 | opcode << 20 | ope1 << 16 | dest << 8 | iv
			print(bin(binary))
			print( "opcode = " + str(opcode) + " &dest = " + str(dest) + " & ope1 = " + str(ope1) + " & iv = " + str(iv))
			return struct.pack('>I',binary)
		else :
			iv = int(instr[3],10)
			binary = 0 << 28 | 0 << 25 | 1 << 24 | opcode << 20 | ope1 << 16 | dest << 8 | iv
			print(bin(binary))
			print( "opcode = " + str(opcode) + " & dest = " + str(dest) + " & ope1 = " + str(ope1) + " & iv = " + str(iv))
			return struct.pack('>I',binary)

def encryptDataProcessingInstr(instr) :
	instr = instr.split(' ')
	opcode = dict_opcode[instr[0]]
	
	#CMP
	if ( opcode == 5 ) :
		binary = cmpInstr(instr, opcode)
		return binary
		
	#MOV
	elif ( opcode == 8 ) :
		binary = movInstr(instr, opcode)
		return binary
		
	#others operations
	elif ( opcode == 0 or opcode == 1 or opcode == 2 or opcode == 3 or opcode == 4 or opcode == 6 or opcode == 7 or opcode == 9 or opcode == 10 ) :
		binary = otherOpeInstr(instr, opcode)
		return binary

def readInstrLine(instr) :
	binary = 0x0

	#branch instruction
	if (instrutionType(instr) == 0) :
		binary = encryptBranchInstr(instr)
		return binary

	#data processing instruction
	elif (instrutionType(instr) == 1) :
		binary = encryptDataProcessingInstr(instr)
		return binary
		

file = sys.argv[1]
fichierasm = open(file,"r")
lines = fichierasm.readlines()

extension = file.split(".")[len(file.split("."))-1]
filename = file.split(".")[len(file.split("."))-2]

if( extension == "s") :
	binaryfile = open("binary","wb")
	for instruction in lines :
		#remove \n & \t
		instruction = instruction.strip('\n').strip('\t')
		if(len(instruction) != 0) :
			binary = readInstrLine(instruction)
			binaryfile.write(binary)
	binaryfile.close()

else : 
	print("Votre fichier doit être un .as pour être lu !!!")



