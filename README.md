## Unix-cp-Command

This is a custom implementation of the Unix `cp` command, which can copy files and directories. It supports recursive copying with the `-r` option and wildcard patterns for copying multiple files.

## Features

- **File Copying**: Copy individual files from a source path to a destination path.
- **Directory Copying**: Use the `-r` option to recursively copy directories.
- **Wildcard Support**: Use wildcard patterns (e.g., `*.txt`) to copy multiple files that match the pattern to a destination directory.

## Usage

### Basic File Copy

```bash
./cp <source_file> <destination_file>
```

Example:
```bash
./cp hello.txt bye.txt
```

### Recursive Directory Copy

``` bash
./cp -r <source_directory> <destination_directory>
```

Recursively copies a directory and its contents from `<source_directory>` to `<destination_directory>`.

Example:

```bash
./cp -r ./dir1/ ./dir2/
```

### Wildcard File Copy

```bash
./cp <wildcard_pattern> <destination_directory>
```

Copies all files that match the wildcard pattern to the specified `<destination_directory>`.

Example:

```bash
./cp *.txt ./test/
```

### Wildcard and Recursive Copying

Currently, wildcard patterns are only supported for copying files, not directories. If you need to copy directories recursively with wildcards, you'll need to do so manually by combining recursive copying with specific file patterns.

## Error Handling

- If the source path does not exist, the program will terminate with an error.
- If attempting to copy a directory without using the `-r` option, the program will print an error and exit.
- If a file or directory cannot be opened or created due to permission issues, the program will print an error and exit.

## Requirements

This program is written in C and should be compiled with a standard C compiler. It uses the following libraries:

- `stdio.h`
- `stdlib.h`
- `string.h`
- `unistd.h`
- `fcntl.h`
- `sys/stat.h`
- `dirent.h`
- `errno.h`
- `fnmatch.h`

## Compilation

To compile the program, use the following command:

```bash
gcc mycp.c -o cp
```

This will create an executable named `cp`.

