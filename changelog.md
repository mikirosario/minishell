## Version 2.2

- Very sloppily written pipe functionality, but pipe functionality nonetheless. xD

### Version 2.15

- Rudimentary pipe implemented. Can only be used once. xD Second process hangs, apparently waiting for input from first process, which never comes as my implementation is sequential and iterative, and write process is already gone... :_( You can leave the process by sending a ctrl-C signal, but to stop the hanging programmatically I've discovered I need to close the ALL the pipe's write fds (i.e. including the parents') so that their associated reference counters reach zero and their associated inode kindly expires... But then, of course, I need to *open them up again* for the following pipe. Not yet implemented, that part. ;)

- The more documentation I read, the more I have a feeling I am implementing this backwards to how it is usually done (execute last command first and have it spawn children recursively). And the more I'm feeling trapped by my own design. *sigh* :p

### Version 2.14

- Shell now knows whether a command called with pipe will only write to the pipe, read from the pipe and write to the pipe, or only read from the pipe, depending on the preceding call. :) The pipe_flag is set to 0 for no pipe, 1 for write-only mode, 2 for read-only mode and 3 for read-write mode. The idea is to use this flag to tell the child processes exactly what to do with the pipe we send them. :)

### Version 2.13

- The cmdline and token structs, previously declared locally within the process_cmdline function and pointed to by pointers in the t_micli struct, are now declared directly within the t_micli struct. All references have been modified accordingly.

- The ft_bzero function is now used to zero the tokdata struct in the process_cmdline function.

- Code clean-up in the memory_free.c file.

- Updated descriptions of process_token and process_cmdline functions.

### Version 2.12

- Removed null_check function and incorporated it back into the process_raw_line while as a set of instructions after cmd_execution.

### Version 2.11

- Fixed null_check so that it no longer increments the line index when it finds a null character.

- Changed the syntax_check function to use toggle_quote_flag function. It continues to use its own local quote flag.

- Changed the process_raw_line to use the toggle_quote_flag function. It continues to use its own local quote flag.

## Version 2.1

- Syntax check is now done before any commands are executed, so a syntax error will prevent any commands in a multi-command line from being executed, as in bash.

- Lines beginning with ';' or '|' will now throw a syntax error.

- Added quote detection in the process_raw_line phase so that ';' or '|' between quotes will no longer be misinterpreted as command line ends. Code is a bit rough and verbose, will streamline it in clean-up process.

- Added basic pipe detection. Shell will now confirm that it has detected a pipe order before executing a command ending with '|' printing a stand-in confirmation message.

- Added micli->pipe array of two integers to contain input and output pipes.

- The process_raw_line function will now use the null_check function to check for end of line, and will now detect that it has reached the end of a line even if the last command in a line ends in ';' or '|'. As we don't process multiline commands, hanging '|' at the end of a line will be ignored.

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

- The linked list (micli->token.var_lst) used to store resolved variable addresses for every token was not being freed and set to null between tokens, causing a segmentation fault when variables were used in multiple arguments to a single command. This has been fixed.

- When a command line ended in a variable and a ';', the ';' was being flagged for deletion along with the variable name by micli_cpy, causing the next command not to be processed. This has been fixed.

- A distinction has been made between variables with validity for a given command+arguments, henceforth 'command line' or 'cmdline', which will now be stored in micli->cmdline, and variables with validity for a given token, henceforth 'token', which will continue to be stored in micli->token. This has been done to clarify when each type of data needs to be reset, as the program parses in strict hierarchical order (see *Basic Overview*).
