#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main() {
    const char *dirname = "."; // 현재 디렉토리

    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);

    return 0;
}
