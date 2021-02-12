# Basic Overview

Minishell parses and processes text from stdin in the manner of bash.

The program parses text in **phases** in the following hierarchical order: *raw line from stdin* -> *command line* -> *token* -> *character*.

Each **phase** of minishell (raw, cmdline, token, character) has its own associated dataset. Character has *micli->tokdata*, token has *micli->token*, command line has *micli->cmdline*, and the raw line uses *micli* directly. Each phase is required to complete all tasks **below** it in the hierarchy before continuing with other tasks **above** it. 

So, each raw line must deal with all of its command lines before exiting, each command line must deal with all of its tokens before exiting, and each token must deal with all of its characters before exiting. Higher phases thus pass tasks down to lower phases as soon as they are identified.

## PHASES

|         RAW LINE         |    CMD LINE  |  TOKEN | CHARACTER |
| ------------------------ | :----------- | :----- | :-------- |
|  $raw line; from stdin   |  $raw line   |  $raw  |     $     |
|           micli          |    cmdline   |  token |  tokdata  |

Subject to change, of course. ;)

# minishell
MICLI - Minishell Project for 42 School Madrid.

The objective of this project is for you to create a simple shell.

* Show a prompt when waiting for a new command

* Search and launch the right executable (based on the PATH variable or by usingrelative or absolute path) like in bash

* It must implement the builtins like in bash:
    * echo with option ’-n’
    * cd with only a relative or absolute path
    * pwd without any options
    * export without any options
    * unset without any options
    * env without any options and any arguments
    * exit without any options

* ; in the command should separate commands like in bash

* ’ and " should work like in bash except for multiline commands

* Redirections < > “>>” should work like in bash except for file descriptor aggregation

* Pipes | should work like in bash

* Environment variables ($followed by characters) should work like in bash

* $? should work like in bash

* ctrl-C, ctrl-D and ctrl-\ should have the same result as in bash
