### Version 4.415

- Checked and fixed (as needed) all reported bugs in the bug report (see bugreport.md).

### Version 4.414

- Fixed a fatal bug that caused a segmentation fault! When resolving undefined variables in the env_var_handling function, it used to be that find_var would ignore them, but find_var was changed to detect them. This led the resolver to treat undefined variables assuming that they had a trailing '=' sign and moving the varp pointer past the result of strchr(varp, '='). In this case, however, strchr returned NULL, because the '=' was not found, and so varp was defined as 0x01. The segmentation fault happened in good old ft_strlen (aka. segmentation fault detector) when it attempted to get the "string length" of 0x01. :p This could have been a DISASTER!!!!!!! Conditions have been introduced to point varp safely to the NUL literal if strchr returns a NULL when searching for the '=' in varp.

- Checked and fixed (as needed) all reported bugs in the bug report up to Case 17 (see bugreport.md).

### Version 4.413

- Checked and fixed (as needed) all reported bugs in the bug report up to Case 10 (see bugreport.md).

### Version 4.412

- Fixed cd bug when HOME unset.

- Fixed bug that prevented escaped spaces or '>' or '<' characters from being registered as such.

### Version 4.411

- Changed termcap string names to avoid conflicts with official curses library.

- Updated arrow keys to user termcaps. Eyeroll.

## Version 4.41

- Updated to use termcaps. Dear. Turing. Termcaps. Why are you like this? O_O

- Updated to support Application Mode.

## Version 4.4

- Norminette completed. Ready for testing and evaluation.

### Version 4.3

- Multiline text in the terminal will now be overwritten properly when scrolling through the command history, regardless of window size.

- Linux compatibility mode introduced to patch out OLDPWD bug when running under Ubuntu 20.04.

- Updated function descriptions.

- Realloc functions have passed the stress test. Set character buffer to 100 and history pointer array buffer to 20.

- Nearly everything has been adapted to Norminette v3 except for a single control structure assignment in cmd_execution.c that I need to look at closely, and after I've gotten some rest. xD

### Version 4.28

- Backspace will now work on multiple lines thanks to a rough but functional implementation. ;)

### Version 4.27

- Tweaked bufsize upper limit to SHRT_MAX - 3, as SHRT_MAX - 3 characters will always fit into the buffer without overwritng the reserveds.

### Version 4.26

- Tweaked buffer size allocation. Changed active_line_size variable name to recalc_bufsize for clarity. Bufsize now consistently means total amount of characters (including scratch character) that will fit in the buffer. Bufsize + 3 shorts are always reserved for any line (for the two data segments and the null terminator). The char_total should never be more than bufsize - 1. Thus now the needed bufsize is given by:
	while (recalc_bufsize / (char_total + 1) == 0)
		recalc_bufsize += READLINE_BUFFER;

### Version 4.25

- Restored null dir check to find_cmd_path as it caused segmentation fault in closedir with null pointers.

### Version 4.24

- More norminette.

### Version 4.23

- The functions in termcaps.c norminetted.

- Fixed signals.

- Norminette.

### Version 4.22

- Added an upper limit of SHRT_MAX - 4 to character buffering, so if SHRT_MAX - 4 characters are loaded into the buffer (that's somewhere north of 32,000 characters), the program will continuously delete the next typed character, preventing an overflow.

### Version 4.21

- Memory reserved for lines saved to the command history stack (aka. recalc_bufsize) is now calculated as a function of the minimum buffer size as follows:
	recalc_bufsize = READLINE_BUFFER;
	while (recalc_bufsize / (char_total + 4) == 0)
		recalc_bufsize += READLINE_BUFFER;
This means that minishell will find the smallest multiple of READLINE_BUFFER in which all of the characters + 1 extra space for an incoming read character + 2 data state items + 1 null character will fit. The reported 'bufsize' at hist_stack[line][1] gives the number of characters that can potentially be held by the current buffer, and so will be recalc_bufsize - 3, that is, the total real size of the buffer minus the null character and the two data items.

This should make the dynamic memory system robust enough to take on any minimum buffer size.

### Version 4.2

- Libft has been moved over to Norminette v3. Note that while critical functions that have been rewritten, like the itoas and split, have been thoroughly tested, get_next_line, which we aren't using here, has been modified without testing for the moment, so be careful. :p

- The function ft_printf has been deprecated in favour of printf, as per the new school standards.

- Fixed bug that caused the first line of user-introduced text in minishell to be green.

- Major rewrite/overhaul of micli_readline, which is now looking MUCH more stable. :) The command history stack lines have been modified to include embedded values about its state. The first two shorts in each stack line are now reserved to store the following information about the line:
	- hist_stack[0] stores the number of characters stored in the line (char_total),
	- hist_stack[1] stores the number of characters that will fit in the line (bufsize).
These values change dynamically whenever characters are added or deleted from any of the lines and whenever a line is reallocated. They are used to curate the lines.

- By using the new line-embedded state values to control memory allocation on a line-by-line basis, I've eliminated a memory misallocation problem introduced with the backspace function. Before the backspace it was possible to use strlen to determine the number of characters in any line and extrapolate the buffer size from that. After the backspace, however, this was no longer viable, as backspaced characters could leave a buffer of nearly any size with any number of characters (I don't reallocate down, as this would be wasteful). Consequently, buffer reallocation became chaotic. After a lot of thinking it over and some doodling in my trusty notebook, I decided to go with this solution of embedding these two state values reporting character count and buffer size in the first two shorts of each line. The great news? This seems to have eliminated the dreaded arrow-crash bug as well, though further testing on that will be needed. :)

- By tying the character total to a short I have effectively limited each line's total potential length to SHRT_MAX. Though shorts allow for lines much longer than should ever reasonably be needed (over 32000 characters), as of present a buffer overflow of this kind is not contemplated in the code, so inputting a line of more than SHRT_MAX - 4 characters will cause a black hole to appear and swallow your computer if attempted. That's not a bug, it's a feature.

- Once further testing is done it will be time to clean up esc_seq and HOPEFULLY that will leave this project in pristine condition. :)

- Fixed bug caused by an unreverted change during testing which inserted DEL markers instead of NUL characters in place of deleted characters in the buffer.

### Version 4.11

- Empty lines will no longer be saved to the history

- Holding down tab defeats tab block - known bug.

### Version 4.1

- Cleaned up the short_to_strdup function, which now lives in its own file called short_to_chars.c.

- Reimplemented single-byte escaping.

- Banned the tab key at last! Inserted easter egg when pressed. ;)

## Version 4.0

- Changed character buffer to shorts as I've found that termios settings only go as far as enabling UTF-8. With UTF-8 enabled and all characters in editable lines now stored in shorts, both one- and two-byte characters can be deleted.

- The character buffer is still converted to a standard char string before being passed on to the parser, so no functions downstream of the termcaps editing layer are affected by this overhaul.

- Increased the maximum number of bytes read at a time by the read function to 2 to fill up the shorts.

- New functions to handle the short strings, including:
	- ft_strlen16, which returns the length of null-terminated 16-bit strings.
	- ft_short_to_strdup, which converts 16-bit (short) strings back to 8-bit (char) strings.
	- is_esc_seq, which detects the escape sequence for arrows enabling scrolling through the command history.
	- push_hist_to_stack, which pushes the line being edited to the cmdhist.hist_stack every new line.
	- del_from_buf, which deletes a character from a cmdhist.hist_stack line.
	- ft_memdup, which functions similarly to ft_strdup, except for arrays of any type and it must be passed the size of the array to be duplicated.
	- ft_free_short_split, to free the cmdhist.hist_stack array.

- The backspace key has been enabled and will now have the effect of removing the character behind the cursor from the screen and from the buffer.

- This version will need extensive bug testing.

### Version 3.5

- Changed character buffer to unsigned ints to enable a grand new invention - character deletion. :p

- Wrote ft_memdup to replace ft_strdup for more general memory duplication use.

### Version 3.42

- Fixed awful bug that caused ';' and '|' to be mistaken for command line ends inside quotes.

- Ambiguous redirect message for echo > test $NADA (undefined variable as file name) not implemented.

### Version 3.41

- Fixed crash that occurred in process_char in the is_escape_char check because since being externalized to its own function is_escape_char takes *(chr + 1) as a parameter without being protected by a null check for *chr first, so chr + 1 is out of bounds when analysing a null char. This has been corrected.

- When cmdline.hist_stack is reallocated, the scratch log of the preceding cmdline.hist_stack is now always freed and replaced with a copy of the active_line. The next null entry in hist_stack will then become the new scratch log.

- The active_line (still named as micli->buffer, after the original buffer) is duplicated immediately after its address is returned by micli_readline and before the duplicate is sent into parsing engine for processing. In pop_to_hist_stack the duplicate of the active line is reduplicated as the last saved (non-scratch) line in cmdline.hist_stack.

- The active_line (micli->buffer) duplicate is freed and nulled after the parsing engine is complete.

- Using the left and right arrows causes memory problems for reasons I don't understand yet. Everything else stable.

### Version 3.4

- Overhauled read buffer. Read buffer now defaults to the penultimate member of a dynamic pointer array called cmdline->hist_stack. The array is a stack of strings. Every time a string is processed with the enter key, it is popped onto the stack. Both the individual strings and the stack itself are reallocated as more space becomes necessary. The buffer size for both is currently set at just 1 member for stress testing purposes.

- Using the up and down arrow keys will now move through the command history of the active session.

- The left and right arrow keys have been effectively disabled. Delete is still not implemented.

- Mysterious occasional double free problem with micli->buffer and occasional heap-use-after-free with ft_split. Problem seems to be ft_split-independent.

- Empty lines are still passed to history stack. This will need to be corrected.

### Version 3.3

- Fixed bad buffer realloc code.

### Version 3.2

- Arrow key escape sequences are now intercepted and amusingly but non-usefully interpreted. Code looks like Lovecraftian horror for the moment.

- Escaped characters will no longer be echoed. Escaped escapes will not escape.

### Version 3.1

- Raw lines are now saved in a buffered array of strings at micli->cmdhist.hist as they are sent in from terminal.

- The function clean_realloc has been modified. It must now be passed both the size of new memory block and the size of the old memory block.

- Canonical mode partially disabled. ECHO disabled, so I am now handling echoing directly. Canonical CRNL and POST and signal handling still active.

## Version 3.0

- The most infamous bug in all of human history, which caused a segmentation fault when PATH was unset and a command was executed, has been fixed. It now behaves as in bash.

- The ability to export or unset variables named '_' has been removed.

## Version 2.9

- Merged and fully normed version.

### Version 2.88

- Normed version, pre-merge.

- Added ft_strncpy to libft.

- Added ft_free_split to libft.

### Version 2.87

- Almost totally normed.

### Version 2.86

- Syntax errors will now set cmd_result to 2, imitating bash functionality.

- The find_cmd_path function will now check the null terminator of the command name string against the binary name as well.

### Version 2.85

- Big Norminette compliance update.

### Version 2.84

- Major clean-up in child_process.c. Cleaned up exit status reporting and collection and removed obsolete variables.

- More Norminette. Miau.

### Version 2.83

- Hanging redirect instructions now return a syntax error.

### Version 2.82

- Fixed bug that prevented the exit status of attempts to execute non-existent files or commands from being saved to micli->cmd_result and thus the built-in variable $?. Thanks Mario. :)

- Fixed bug that caused partial matches to built-in commands to register as found built-ins.

- Modified the close_pipe function to close all fds individually. I originally wrote it when I harboured a misunderstanding of file structures, thinking there were only as many file structures as there were files.

- Separated the part of the pipe_reset function that freed pipe memory and zeroed pipe data members into a clear_pipe function that also calls close_pipes to leave pipe_reset the exclusive task of reestablishing a pipeline.

- Separated the part of the exit_failure function that printed system or unknown errors into a seprate sys_error function located in iamerror.c and now callable from elsewhere in the program.

### Version 2.81

- I'm not proud of the workaround I came up with here for the MacOS error reporting issue for broken pipelines. Yes... it's a while that does nothing but kill cycles until the child has a chance to exit_failure before checking its status. No, it's not my proudest moment. xD But deadlines are deadlines. :p

- Bug that caused pipelines (|) and command line ends (;) not to be recognized after an escaped escape character fixed. echo \\| wc and echo \\; echo test now return the same result as bash. Thanks for spotting this one, Julen. :)

### Version 2.8

- War against hanging pipeline bug concludes with allied victory. A variable has been introduced to save the waitpid status of each command after launch. If any of them fail to launch it will be returned to a local child_res variable, and the exit status will be used to print an error message. Currently there is only one error message that assumes command not found or no such file or directory, but this could be expanded by using stat_loc for a more detailed status.

- Amazingly, it did not occur to me that execve might just fail and return control of the calling process back to my child. I guess I just thought it would close the process all by itself. Instead, I found to my amazement as I implemented the patch to handle broken pipelines, my child would then go on its own merry way, doing all the same things that its parent would do... twice over, thrice over, as many times over as orphaned processes I left in my wake. xD D'oh! To stop this disastrous behaviour, I have introduced a failure condition after execve so the program is still closed. Don't judge me, OK, this is why I'm a student. xD

### Version 2.76

- Norminette continues.

- War against hanging pipeline bug in progress.

### Version 2.75

- Norminette continues.

- Known Bugs:
- /bin/echoasfd a | cat (if command in pipeline doesn't exist and isn't last command, pipeline hangs)
- echo > hanging redirect instruction doesn't return syntax error
- If characters are typed while process is executed, they will be read from stdin in next iteration.

### Version 2.74

- Reorganized process_cmdline, process_raw_line, process_char to clean up the code. Some additional files added.

### Version 2.73

- Updated syntax_check to ban lines starting in '>' or '<'.

- Note case: echo 2; < ; echo 4

- Massive norminette progress.

### Version 2.72

- Fixed export_print bug.

### Version 2.71

- Rewrote delete_oldpwd function in the process of testing the Linux/make bug. Confirmed that make does not execute properly when OLDPWD is undefined in the envp array. Still no idea why.

## Version 2.7

- CD: Removed superflous variables.

- EXPORT: Fixed sorting function export_order to return an ordering mask of size_t integers that is now used to print envp in alphabetical order without changing its actual order in memory.

- UNSET: Fixed unset so that it... eh, actually works. ;p It will re-allocate the pointers in the envp list now when called and remove the old pointer list. Not the most efficient implementation ever, but functional, and the deadline looms...

- Detected strange bug in Ubuntu Linux 20.04 Focal where "make" will not execute unless cd is used first. May have something to do with OLDPWD or PWD. Will try to reproduce at 42 on my laptop, though my laptop runs Ubuntu 18.04 LTS. Does not seem to happen on MacOS, but will check again.

### Version 2.66

- Builtin bugfixes. The cd function now correctly resolves '~' to home directory in all cases ("~", "~/" and "~/PATH").

- Export now takes multiple arguments.

- Env now displays error message if passed an argument, as in bash.

### Version 2.65

- The function cmd_execution has been modified to launch export within the parent process when it is called without arguments.

- The ctrl-D (EOF) signal is now working.

### Version 2.64

- All builtins are executed locally except echo. (Note export will need to be split into local and child parts).

- Signal experiments.

### Version 2.63

- By freeing memory before execve I destroyed argv and envp before sending them, causing chaos. Sorry.

- Some builtins need to be executed locally rather than in children to function properly (export, cd, etc.). Not yet implemented.

### Version 2.62

- The new generalized clean_realloc has been further modified to be more like the old realloc. It only need to be told the size of the memory to be reallocated, but will now work with any kind of array. The new generalized clean_realloc has now completely replaced the old one.

- Memory duplicated in children is now explicitly freed before exiting/repurposing their processes.

### Version 2.61

- Length of "🚀 " incorrectly given to write as '6', now corrected to '5'.

- Rewrote micli_read loop to better use the read buffer and reduce the instructions. Now READLINE_BUFSIZE characters at a time will be read and copied to the buffer instead of 1 character at a time. The buffer will be reallocated with an additional READLINE_BUFSIZE if more bytes need to be copied, as before.

- Removed the linked list that copied and saved sequestered redirect instructions for debugging. File paths are still copied before being sent to the open command.

- I've written a more general version of clean_realloc to work with any kind of array, not just a character string, but it now needs to be told what the size of the array members will be. It hasn't yet been implemented so as not to break functions on other branches that use the prior implementation. Implementation will be coordinated in a future update.

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

### Version 2.4-a

- Modified find_var to work with both defined and undefined variables.

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

- The read buffer has been increased to 1024 from 1, as, after weeks of stress-testing without failures, I now consider clean_realloc to have proven itself. ;)

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
