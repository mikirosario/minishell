### Version 2.032

- Code required for builtin exit status was accidently left commented in the last push. :p

### Version 2.031

- The special '?' variable now resolves to the exit status of the last executed command both for binary executables and builtins.

### Version 2.03

- A special '?' variable has been added that resolves to the exit status of the last executed command if the command points to a binary. If the command does not exist, it resolves to 127. NOTE: This still does not work with the builtins as the builtins still do not return their exit status.

### Version 2.02

- The exec_cmd function in cmd_execution.c will now print an error message if a specified command is found neither among the builtins nor among the system binaries.

- Commented out variable that is unused since Version 2.01: micli->builtin_strlen.

### Version 2.01

- Switched builtin detection in the find_cmd_path function over to using the find_builtin function (both defined in cmd_execution.c). With this update, only comma-separated words within the BUILTIN string will be recognized as builtins. Partial matches will not be misinterpreted as full matches (i.e. "namels," will not be misinterpreted as a match for "ls").

- Moved function exec_builtins from cmd_execution.c to builtins.c so that the builtins branch of development is entirely self-contained.

- Updated exec_cmd function in cmd_execution.c to use the find_var function to find the PATH variable, rather than the simple provisional while that existed before. This will ensure that variables with a partial match for PATH (i.e. PATH_MORETEXT) are not mistaken for the PATH variable.

## Version 2.0

- Updated the get_var_lengths description to be more accurate.

- The linked list (micli->token->var_lst) used to store resolved variable addresses for every token was not being freed and set to null between tokens, causing a segmentation fault when variables were used in multiple arguments to a single command. This has been fixed.

- When a command line ended in a variable and a ';', the ';' was being flagged for deletion along with the variable name by micli_cpy, causing the next command not to be processed. This has been fixed.

- A distinction has been made between variables with validity for a given command+arguments, henceforth 'command line' or 'cmdline', which will now be stored in micli->cmdline, and variables with validity for a given token, henceforth 'token', which will continue to be stored in micli->token. This has been done to clarify when each type of data needs to be reset, as the program parses in strict hierarchical order (see *Basic Overview*).
