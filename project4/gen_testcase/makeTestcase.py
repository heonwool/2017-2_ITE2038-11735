import random

INSERT_CMD_FMTS = "i %d %s\n"
FIND_CMD_FMTS = "f %d\n"

f = open("100_ins2.txt", "w")

case_range = list(range(100))
#random.shuffle(case_range)

for i in case_range:
	input_d = INSERT_CMD_FMTS % (i * 2, 'ins2_' + str(i * 2))
	f.write(input_d)

f.close()

