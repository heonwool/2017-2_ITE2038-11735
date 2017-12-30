import random

INSERT_CMD_FMTS = "i %d %s\n"
FIND_CMD_FMTS = "f %d\n"
DELETE_CMD_FMTS = "d %d\n"

f = open("rand_2^20_ins.txt", "w")

case_range = list(range(2**20))
random.shuffle(case_range)

for i in case_range:
	input_d = INSERT_CMD_FMTS % (i, 'string' + str(i))
	f.write(input_d)

f.close()

f = open("rand_2^20_find.txt", "w")
	
for i in case_range:
	input_d = FIND_CMD_FMTS % (i)
	f.write(input_d)

f.close()

f = open("rand_2^20_del.txt", "w")
	
for i in case_range:
	input_d = DELETE_CMD_FMTS % (i)
	f.write(input_d)

f.close()
