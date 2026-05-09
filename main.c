#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Count of args: %d\n", argc - 1);

    if (argc > 1) {
        printf("Args:\n");
        for (int i = 1; i < argc; i++) {
            printf("%d: %s\n", i, argv[i]);
        }
    } else {
        printf("No args.\n");
    }

    return 0;
}