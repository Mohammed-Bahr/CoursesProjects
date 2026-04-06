This is a **Java-based Terminal/Shell Emulator** that mimics a command-line interface with file system operations, I/O redirection, and archive support. Here's the comprehensive documentation:

## Architecture Overview

```
Terminal (Main Container)
├── Parser (Inner Class) - Command tokenization & parsing
├── State Management (currentDirectory)
└── Command Executors - 15+ shell commands
```

---

## 1. The Parser Class

**Purpose**: Tokenizes user input using regex to handle quoted strings and arguments.

**Key Logic**:
```java
// Regex breaks input into tokens:
// - Quoted strings: "file name with spaces"
// - OR non-whitespace sequences: \S+
Pattern pattern = Pattern.compile("\"([^\"\\\\]*(?:\\\\.[^\"\\\\]*)*)?\"|(?:\\S+)");
```

**Special Handling**:
- Detects `-r` flag immediately after command (e.g., `cp -r`, `zip -r`)
- Strips quotes from arguments (handled in individual commands)
- Returns: `commandName`, `args` vector, `fullCommand`

---

## 2. Command Execution Flow

```
User Input → Parser.parse() → chooseCommandAction()
                                    ↓
                         Check for Redirection (> or >>)
                                    ↓
                    executeWithRedirection() OR executeCommand()
                                    ↓
                              Switch Statement
                                    ↓
                            Specific Command Method
```

**Redirection Support**:
- `>` : Overwrites output to file
- `>>` : Appends output to file
- Uses `System.setOut()` to swap stdout temporarily

---

## 3. Command Reference

### Navigation
| Command | Implementation Details |
|---------|----------------------|
| `pwd` | Prints `currentDirectory` (absolute path) |
| `cd [dir]` | Supports `".."`, absolute paths, quoted paths with spaces. Changes `currentDirectory` field |
| `cd` (no args) | Jumps to user home directory (`System.getProperty("user.home")`) |

### File/Directory Management
| Command | Details |
|---------|---------|
| `ls` | Lists readable files (sorted alphabetically), no args supported |
| `mkdir <dir...>` | Creates multiple directories, handles quoted paths, validates parent exists |
| `rmdir <dir\|*>` | Removes empty directory OR all empty directories in current dir if `*` |
| `touch <file>` | Creates empty file or warns if exists (won't overwrite) |
| `rm <file>` | Deletes single file only (not directories), validates regular file |

### File Operations
| Command | Details |
|---------|---------|
| `cat <file> [file2]` | Reads all lines, prints content with `---------- End of filename ---------` separator |
| `cp <src> <dest>` | File-to-file copy only, validates dest exists and is writable, preserves attributes |
| `cp -r <src> <dest>` | Recursive directory copy using `Files.walkFileTree()`, preserves structure |
| `wc <file>` | Counts lines, words (split by `\s+`), characters (+1 for newline) |

### Archive Operations
| Command | Details |
|---------|---------|
| `zip <archive> <file1> [file2...]` | Creates `.zip` with multiple files (auto-adds extension) |
| `zip -r <archive> <dir>` | Recursive zip including subdirectories, entry format: `dirname/subdir/file` |
| `unzip <archive> [dest]` | Extracts to current dir or specified destination, creates dirs as needed |

### I/O
| Command | Details |
|---------|---------|
| `echo <args...>` | Prints arguments space-separated |
| `help` | Static method listing all commands |

---

## 4. Path Handling Strategy

**Consistent Pattern Across Commands**:
1. Strip surrounding quotes if present: `"file name"` → `file name`
2. Resolve relative paths against `currentDirectory`
3. Normalize paths (resolve `.` and `..`)
4. Convert to absolute paths for validation

```java
Path target = Paths.get(input);
Path final_path = target.isAbsolute() ? target : currentDirectory.resolve(target);
final_path = final_path.toAbsolutePath().normalize();
```

---

## 5. Error Handling Approach

- **Input Validation**: Checks argument count, file existence, directory vs. file types
- **Exception Hierarchy**: Catches specific exceptions (`NoSuchFileException`, `InvalidPathException`, `IOException`)
- **User Feedback**: Descriptive error messages (e.g., `"cp: cannot copy 'X': Not a regular file"`)
- **Safety Checks**: 
  - Prevents `cp`/`cp -r` if source = destination (`Files.isSameFile()`)
  - `rm` refuses to delete directories
  - `rmdir` refuses non-empty directories

---

## 6. Key Implementation Features

### Recursive Copy (`cp -r`)
Uses `SimpleFileVisitor<Path>` pattern:
- `preVisitDirectory`: Creates corresponding dir in destination
- `visitFile`: Copies file with `REPLACE_EXISTING` flag

### Recursive Zip (`zip -r`)
Uses `Files.walk()` stream to filter regular files:
- Entry names preserve relative structure: `rootDir/subdir/file.txt`
- Converts Windows backslashes to forward slashes for zip compatibility

### Redirection System
```java
// Redirects stdout to file stream
PrintStream fileOut = new PrintStream(new FileOutputStream(path, append));
System.setOut(fileOut);
executeCommand(...);  // All System.out.println go to file
System.setOut(originalOut);  // Restore in finally block
```

---

## 7. Usage Examples

```bash
# Navigation
documents > cd "My Folder"
My Folder > pwd
/home/user/documents/My Folder

# File operations
My Folder > touch "new file.txt"
My Folder > ls
new file.txt
My Folder > echo Hello World > "new file.txt"
My Folder > cat "new file.txt"
Hello World
---------- End of new file.txt ---------
My Folder > wc "new file.txt"
1 2 12 new file.txt

# Archives
My Folder > zip backup.zip "new file.txt"
  adding: new file.txt
zip: created archive 'backup.zip'
My Folder > unzip backup.zip ../extracted
  extracting: new file.txt
unzip: extracted archive 'backup.zip' to '../extracted'

# Redirection
My Folder > ls > files.txt
My Folder > echo "new line" >> files.txt
```

---

## 8. Limitations & Design Notes

1. **No Wildcards/Globbing**: `*` only works specifically in `rmdir *`, not general glob support
2. **Single-threaded**: No background process support (`&` not implemented)
3. **Pipes**: No `|` support between commands
4. **Permissions**: Basic read/write checks but no granular Unix permission handling
5. **Memory**: `cat` and `wc` use `Files.readAllLines()` - loads entire file into memory
6. **CP Destination Behavior**: Requires destination file to exist (unlike Unix `cp` which creates it)

