#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("example.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    const char *data = "Hello, System Call!\n";
    ssize_t num_written = write(fd, data, strlen(data));
    if (num_written == -1) {
        perror("write");
        return 1;
    }

    close(fd);
    return 0;
}
