### Version 2.61

- Length of "🚀 " incorrectly given to write as '6', now corrected to '5'.

- Rewrote micli_read loop to better use the read buffer and reduce the instructions. Now READLINE_BUFSIZE characters at a time will be read and copied to the buffer instead of 1 character at a time. The buffer will be reallocated with an additional READLINE_BUFSIZE if more bytes need to be copied, as before.

- Removed the linked list that copied and saved sequestered redirect instructions for debugging. File paths are still copied before being sent to the open command.

- I've written a more general version of ft_realloc to work with any kind of array, not just a character string, but it now needs to be told what the size of the array members will be. It hasn't yet been implemented so as not to break functions on other branches that use the prior implementation. Implementation will be coordinated in a future update.

## Version 2.6

- Consolidated all command executions (builtins, pipes, redirects) in a single child type, currently with a single function.

- Redirects ended by a pipe to another command now handled as in bash.

- Moved fd_redir_flags back to cmdline struct as its lifespan is that of a command, not a token.

## Version 2.5

- The program now handles file redirects, creating and closing new files as needed.

- Redirects ended by a pipe to another command are still not handled as in bash. The piped command is executed as if unpiped and hangs waiting for input.

### Version 2.42

- The program now sequesters '<', '>' and '>>' instructions along with file names. I may supress the redir_token linked list entirely when done with debugging... shouldn't be necessary once I get the fds.

- The program now opens files in read, write trunc and write append mode depending on the instruction preceding the file name.

### Version 2.41

- Introduced a function called find_redir_end that, when passed a pointer to the beginning of a string of redirect instructions, finds and returns a pointer to the end of the redirect instructions within that string.

- Introduced a mode into process_char that detects the beginning of a series of redirect instructions and sets a flag to mark it, and another flag to mark the end of the redirect instructions.

- The process_token function now uses the pointers to the start and end of redirect instructions to sequester all file paths within a redirect string into a linked list pointed to by micli->cmdline.redir_tokens, for later use to create/open files as needed. Consequently, redirect strings are no longer taken by micli as arguments. Arguments both before and after a redirect string are taken and passed to the original command, as in bash.

- Note that the byte of a leading '>' or '<' is still counted before being sequestered, so the debugger still claims it as '1 byte reserved', but memory is not actually allocated for it and no linked list member is associated with it.

- Added a bzero instruction to clear_cmdline. May move the bzero instructions from start of process_cmdline function to dedicated clear functions, though presently I've only done this for the clear_cmdline bzero, which is now duplicated as a result.

- Created a much needed 'rebug' instruction in the Makefile to fclean and recompile in debug mode. ;)

## Version 2.4

- Builtins export and env now partially functional and working with both defined and undefined variables as in bash.

- Builtin pwd functional.

- Builtins have been moved into their own directory.

- Note, builtins are not yet spawned into child processes like normal commands, so they still cannot be piped.

### Version 2.35

- Minishell now accepts absolute and relative paths with '/path', './path', '../path' and '~/path'.

### Version 2.34

- Incorporated Miguel's envp_dup function to make a copy of the originally inherited envp array.

- The plan is to remove undefined variables from the envp array copies whose pointers will be sent to child processes, but this is not yet implemented.

### Version 2.33

- The process_char function has been updated to account for the following newly discovered special variable names: '!', '@', '$' and any digit, though they will always resolve to a null character at present. '$$' should resolve to the PID of the running shell instance, but I don't know what the rest are for.

- The variable flag has been overhauled and is now a pointer to the character after the last character in the variable name, to simplify variable flagging and unflagging. The variable flag will now be reset by a single check for the end of a variable name at the beginning of process_char.

- The process_char function has been totally overhauled and given its own file, process_char.c. The special character conditions in process_char have been functionalized for clarity and some logical formulations have been inverted to optimize CPU use by placing more likely determining conditions first. Descriptions have been updated. Old code has been left commented for now in case bugs are discovered and comparative testing is needed.

- The var_alloc function has been modified to permit the resolution of variables with names beginning with a digit to more closely replicate bash functionality.

- This update bashifies the parsing results for the following lines:<br/>
	echo $unlistedvar*<br/>
	echo $9<br/>
	echo $9283<br/>
	echo $@n<br/>
	echo $!n<br/>

### Version 2.32

- Redundant null check in pipe_count eliminated.

- Updated the process_char function and the process_raw_line function as follows:
	'\' will no longer escape single quotes when single quotes are open, but will continue to escape double quotes when double quotes are open.
	'\' will only be treated as an escape character between double quotes when escaping '"', '$' or '\'.

- This update bashifies the parsing results for the following lines:<br/>
	echo "test \\| test"<br/> 
	"\lol"<br/> 
	\lol<br/> 
	echo test \" | cat<br/> 
	echo test \" | echo test \"<br/> 

- Escapable characters between double quotes have been put into a constant called DQUOTE_ESC_CHARS.


### Version 2.31

- The pipe array will now only reserve as many pipes as needed, rather than one more than needed as it did before. The equation used to derive the proper write and read pipe_fds from the pipe number has been modified to work with the new pipe array.

- The pipe_reset function has been protected and will now fail in case more pipes are called than associated file descriptors can fit in a size_t integer (that is, SIZE_MAX / 2 - 1 pipes, if my maths do not fail me - though I will try to get the resident math geniuses to double check this ;)). SIZE_MAX / 2 - 1 has thus been made a constant called PIPE_MAX.

- Bug in the pipe_reset function caused already-created pipes not to be closed properly in case of pipe creation failure. This has been fixed.

- The close_pipes function has been made independent of pipe_reset. Both live in the pipe_handling.c file.

- The close_pipes function will now also abort minishell if any pipe closure fails. If pipe creation and pipe closure both fail in succession, minishell will display the pipe closure failure as its error before terminating.

- An A || B check in process_line has been simplified to a single null character check.

## Version 2.3

- Pipes now execute all commands in the pipeline concurrently (though they are still *called* sequentially), and the three-pipe system has been replaced with an n-pipes system.

- The function pipe_count had to be modified to take ';' into account.

- The read buffer has been increased to 1024 from 1, as, after weeks of stress-testing without failures, I now consider ft_realloc to have proven itself. ;)

### Version 2.21

- Cleaned up pipe code, which has been consolidated into its own file, pipe_handling.c.

-  Unfortunately, while this **is** functional for *sequentially executed commands*, it is **not functional** for *concurrently executed commands*. That is, each process must close before the next one is executed. So this will not replicate bash for example, in the case of *cat /dev/urandom | less*, where *less* will only display the results of *cat /dev/urandom* if *cat /dev/urandom* is terminated. Mario came up with this experiment, and performing it showed me exactly *why* implementations of piping are normally not sequential. :p Can my implementation be saved...? Tune in for the next riveting episode of minishell to find out... I will have to abandon the three-pipe system at the very least in favour of an n-pipes system.

## Version 2.2

- Very sloppily written pipe functionality, but pipe functionality nonetheless. xD.

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
