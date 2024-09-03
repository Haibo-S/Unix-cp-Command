#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>

void handleError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void copyFile(const char *srcPath, const char *destPath) {
    int fdSrc, fdDes;
    char *readBuf = NULL;
    
    fdSrc = open(srcPath, O_RDONLY);
    if (fdSrc == -1) {
        handleError("Failed to open source file");
    }

    size_t size = lseek(fdSrc, 0, SEEK_END);
    if (size == (size_t)-1) {
        handleError("Failed to determine file size");
    }
    
    lseek(fdSrc, 0, SEEK_SET);

    readBuf = (char *)malloc(sizeof(char) * size);
    if (readBuf == NULL) {
        handleError("Memory allocation failed");
    }

    ssize_t n_read = read(fdSrc, readBuf, size);
    if (n_read == -1) {
        handleError("Failed to read file");
    }

    fdDes = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fdDes == -1) {
        handleError("Failed to open destination file");
    }

    ssize_t n_write = write(fdDes, readBuf, size);
    if (n_write == -1) {
        handleError("Failed to write to file");
    }

    close(fdSrc);
    close(fdDes);
    free(readBuf);
}

void copyDirectory(const char *srcDir, const char *destDir) {
    DIR *dir = opendir(srcDir);
    if (dir == NULL) {
        handleError("Failed to open source directory");
    }

    if (mkdir(destDir, 0700) == -1 && errno != EEXIST) {
        handleError("Failed to create destination directory");
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char srcPath[PATH_MAX];
        char destPath[PATH_MAX];
        snprintf(srcPath, sizeof(srcPath), "%s/%s", srcDir, entry->d_name);
        snprintf(destPath, sizeof(destPath), "%s/%s", destDir, entry->d_name);

        struct stat st;
        if (stat(srcPath, &st) == -1) {
            handleError("Failed to stat source path");
        }

        if (S_ISDIR(st.st_mode)) {
            copyDirectory(srcPath, destPath);
        } else {
            copyFile(srcPath, destPath);
        }
    }

    closedir(dir);
}

void copyWildcardFiles(const char *pattern, const char *destDir) {
    char dirName[PATH_MAX];
    char *slash = strrchr(pattern, '/');
    
    if (slash) {
        strncpy(dirName, pattern, slash - pattern);
        dirName[slash - pattern] = '\0';
        pattern = slash + 1;
    } else {
        strcpy(dirName, ".");
    }

    DIR *dir = opendir(dirName);
    if (dir == NULL) {
        handleError("Failed to open directory for wildcard matching");
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (fnmatch(pattern, entry->d_name, 0) == 0) {
            char srcPath[PATH_MAX];
            char destPath[PATH_MAX];
            snprintf(srcPath, sizeof(srcPath), "%s/%s", dirName, entry->d_name);
            snprintf(destPath, sizeof(destPath), "%s/%s", destDir, entry->d_name);
            copyFile(srcPath, destPath);
        }
    }

    closedir(dir);
}

int main(int argc, char** argv) {
    struct stat st;

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s [-r] <source> <destination>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int recursive = 0;
    char *srcPath;
    char *destPath;

    if (argc == 4) {
        if (strcmp(argv[1], "-r") == 0) {
            recursive = 1;
            srcPath = argv[2];
            destPath = argv[3];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        srcPath = argv[1];
        destPath = argv[2];
    }

    if (recursive) {
        if (stat(srcPath, &st) == -1) {
            handleError("Failed to stat source path");
        }

        if (S_ISDIR(st.st_mode)) {
            copyDirectory(srcPath, destPath);
        } else {
            fprintf(stderr, "cp: -r not specified; omitting directory '%s'\n", srcPath);
            exit(EXIT_FAILURE);
        }
    } else {
        if (strchr(srcPath, '*') != NULL) {
            copyWildcardFiles(srcPath, destPath);
        } else {
            if (stat(srcPath, &st) == -1) {
                handleError("Failed to stat source path");
            }

            if (S_ISDIR(st.st_mode)) {
                fprintf(stderr, "cp: cannot copy a directory '%s' without -r\n", srcPath);
                exit(EXIT_FAILURE);
            } else {
                copyFile(srcPath, destPath);
            }
        }
    }

    return 0;
}
