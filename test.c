#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main() {
    const char *relativePath = "/opwo/../../hamza";

    // Allocate a buffer to store the resolved path
    char resolvedPath[PATH_MAX];

    // Use realpath to get the absolute path
    char *result = realpath(relativePath, resolvedPath);

    if (result != NULL) {
        printf("Absolute Path: %s\n", resolvedPath);
    } else {
        perror("realpath");
        exit(EXIT_FAILURE);
    }

    return 0;
}
