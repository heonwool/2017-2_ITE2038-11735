#include "bpt.h"

// MAIN

int main( int argc, char ** argv ) {

    char * input_file;

    int input, result;
    char instruction, *value, *result_str = NULL;
    value = (char *)calloc(120, sizeof(char));


    if (argc > 1) {
        input_file = argv[1];
        open_db(input_file);
    }


    printf("> ");
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 'd':
            scanf("%d", &input);
            result = delete(input);
            break;
        case 'i':
            scanf("%d %s", &input, value);
            result = insert(input, value);
            break;
        case 'f':
            scanf("%d", &input);
            result_str = find(input);
            if(result_str != NULL) printf("Key: %d, Value: %s\n", input, result_str);
            else printf("Key %d not exists\n", input);
            fflush(stdout);
            break;
        case 'x':
            terminateDBfile();
            return EXIT_SUCCESS;
            break;
        }
        while (getchar() != (int)'\n');
        printf("> ");
    }
    printf("\n");

    return EXIT_SUCCESS;
}
