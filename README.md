This project performs directory manipulation tasks such as copying directories, deleting files and folders, and synchronizing directories by checking for missing files. It includes multiple modules for supporting the main operations in the `quic.c` program. The program is controlled via command-line arguments, and the behavior can be customized through a `Makefile`.

### Key Features:
- **Copying Directories**: Copies the source directory to the destination directory while preserving file structure, permissions, and timestamps.
- **Deleting Files/Folders**: Deletes any files or directories from the destination that no longer exist in the source.
- **Command-Line Arguments**: Accepts arguments to define source and destination directories, as well as other configurations like the directory path.
- **Error Checking**: Ensures the source directory exists and handles any errors in the process.
- **Logging**: Outputs logs for various operations including copying, deletion, and error reporting.

## Setup & Usage

### Directory Structure
- `include/`: Contains header files for function declarations.
- `modules/`: Contains supporting functions for directory manipulation.
- `programs/`: Contains the main program `quic.c` and the `Makefile`.

### Compilation & Execution

1. **Compiling the Program**:
   To compile the project and generate the executable `quic`, run:
   ```bash
   $ make
2. **Running the Program**:
    To run the program with the current arguments, use:
    ```bash
    $ make run
  To modify the arguments, edit the ARGS variable in the Makefile.

3. **Debugging with Valgrind**:
   To run the program with Valgrind for debugging, use:
   ```bash
   $ make valgrind
4. **Cleaning Up**:
    To clean up the generated files (executables and object files), use:
    ```bash
    $ make clean

## Main Functions

  - `copyDir`: Copies directories and files from the source to the destination, ensuring identical structures and permissions.
  - `deleteDirOrFiles`: Deletes files or directories in the destination that are no longer present in the source.
  - `checkCommandArguments`: Validates the command-line arguments for correctness.
