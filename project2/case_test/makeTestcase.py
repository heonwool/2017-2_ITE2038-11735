import random

INSERT_CMD_FMTS = "i %d %s\n"
FIND_CMD_FMTS = "f %d\n"

f = open("del_01_rand_ins.txt", "w")

case_range = list(range(100))
random.shuffle(case_range)

for i in case_range:
	input_d = INSERT_CMD_FMTS % (i, 'string' + str(i))
	f.write(input_d)
	
for i in case_range:
	input_d = FIND_CMD_FMTS % (i)
	f.write(input_d)

f.close()

f = open("del_01_rand_find.txt", "w")
	
for i in case_range:
	input_d = FIND_CMD_FMTS % (i)
	f.write(input_d)

f.close()
