#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int main() {

	FILE *fp = fopen("data.db", "a+");
	
	int64_t test1=12, test2=20, test3=300;
	int64_t test4=489, test5=56, test6=7;

	fprintf(fp, "%" PRId64 "\n", test1);
	fprintf(fp, "%" PRId64 "\n", test2);
	fprintf(fp, "%" PRId64 "\n", test3);
	fprintf(fp, "%" PRId64 "\n", test4);
	fprintf(fp, "%" PRId64 "\n", test5);
	fprintf(fp, "%" PRId64 "\n", test6);

	fclose(fp);

	return 0;
}
