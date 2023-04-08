# List - A `ls` Implementation

`list` is a simple implementation of the `ls` command, written in C. It allows you to list files in a specified directory, with a variety of options for filtering.

## Usage

To use `list`, you can pass 0 arguments to list files in the current directory, a specified file to list only that file, or a specified directory to list all files in that directory.

```
list [OPTIONS] [FILE/DIR]
```

## Options

- `-r`: **R**ecursively list subdirectories.
- `-a`: List **a**ll files, including hidden files.
- `-l MIN_SIZE`: Specify the **l**east file size (in bytes or human-readable format like `1MB`) to display.
- `-h MAX_SIZE`: Specify the **h**ighest file size (in bytes or human-readable format like `1MB`) to display.
- `-m N_DAYS`: Specify the number of days within which files have been **m**odified (files modified within the last N days).
- `--`: Terminate command option parsing. This allows you to work with files that have names starting with '-'.

## Examples

1.  List all files in the current directory:

```
list
```

2.  List a specified file:

```
list myfile.txt
```

3.  List all files in a specified directory:

```
list /path/to/directory
```

4.  Recursively list all files in a directory and its subdirectories:

```
list -r /path/to/directory
```

5.  List all files, including hidden files:

```
list -a
```

6.  List files with a minimum size of 1KB and a maximum size of 1MB:

```
list -l 1024 -h 1048576
```

7.  List files modified within the last 7 days:

```
list -m 7
```

8.  Terminate option parsing and list a file with a name starting with '-':

```
list -- -myfile.txt
```
