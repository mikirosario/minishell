/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/01 20:47:05 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/04 00:49:01 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"




/*
** The purpose of this function is to find the end of a set of redirect
** instructions within a command line string, which we save as a reference to
** unset the redirect flag that causes the process_token function to sequester
** redirect instructions from the argument list so that they are not sent to the
** command as arguments.
**
** Yes, it's complicated. Yes, I did redirects last. xD
**
** Redirect strings are considered to end when the redirect flag is set and an
** unescaped redirect command is NOT found after the last file path is
** processed. File paths are considered to end on unescaped spaces, end of
** cmdline (unescaped ';' or '|'), or null meaning EOL/EOF.
**
**			 set	     unset
**		 redir_flag	  redir_flag
**			  ↑			   ↑
** cmd arg arg<file > file arg arg
**			  ↓			   ↓
**			start		  stop
**			redir		  redir
**			sequester	  sequester
**
** Currently the bastards are sequestered onto a linked list, but I might change
** that to a single-line buffer further down so I can parse it easier. Depends
** on how I dig myself out of this paradigmatic mess. xD For now they're not
** bothering the command, at least. xD
*/

char *find_redir_end(char *redir_str)
{
	unsigned char	escape_flag;

	escape_flag = 0; //this is a subparser, so it has its own escape flag
	//Condiciones de salida: estas condiciones determinan FIN DEL INFIERNO; DIGO FIN DE INSTRUCCIONES DE REDIRECCIÓN
	// NULL		 	O 	NO ESCAPADO			';',			'|',					' ',				
	//(!*redir_str || !escape_flag && (*redir_str == ';' || *redir_str == '|' || *redir_str == ' ')
	
	//set instruction, get file name
	//this needs to determine where INSTRUCTION SET ENDS... IT MUST IT MUST
	//echo "2*3" >> mult \; yoquese <
	//echo "2*3" >> mult ; yoquese
	//WHERE DOES THE DAMN INSTRUCTION SET END??? IT ENDS IF WE CAN'T FIND MORE INSTRUCTIONS AFTER THE LAST FILE_NAME
	while (*redir_str == '>' || *redir_str == '<') //So long as we keep finding new instructions...
	{
		//found instruction, now skip instruction
		if (*redir_str == '>' && *(redir_str + 1) == '>') //if it's a double thingy
			redir_str += 2; //increment pointer past the ">>"
		else
			redir_str += 1; //otherwise increment pointer past the '>' or '<'
		redir_str = ft_skipspaces(redir_str); //now skip any spaces, spaces are legit in redirect instruction world as long as there are more redirect instructions to find...
		if (!*redir_str || *redir_str == '>' || *redir_str == '<' || *redir_str == ';' || *redir_str == '|') //if we find a null, another '<' or '>', ';', '|'
			return (redir_str); //this would be a syntax error so it should end the redirect instructions... maybe return NULL and skip it all?
		//found file name, now find end of file name
		//Condiciones de salida
		//!*redir_str || !escape_flag && (*redir_str == '>' || *redir_str == '<' || *redir_str == ' ' || *redir_str == ';' || *redir_str == '|')
		//Condiciones de seguir (lógica inversa)
		while (*redir_str && (escape_flag || (*redir_str != '>' && *redir_str != '<' && *redir_str != ' ' && *redir_str != ';' && *redir_str != '|')))
		{
			if (!escape_flag && *redir_str == '\\')
				escape_flag = 1;
			else
				escape_flag = 0;
			redir_str++;
		}
		//find next instruction, or die trying
		redir_str = ft_skipspaces(redir_str); //skip any spaces... we should find either another instruction, or something else
		//if we find another instruction, rinse and repeat, if not, it's all over, we found the end!
	}
	return (redir_str); //return end of detected instruction set	
}

/*
** Note: Bash behaviour is first to open a requested file, then ask questions
** once the entire command line is processed.
**
** In the case of: echo "2*3" >mu>lt, bash creates/opens mu, then creats/opens
** lt, and ultimately passes the data to lt as it is the last fd opened. So we
** need to do that too. ;)
**
** This function is currently a basket case, so kindly look away... :p
*/

// char	*retrieve_redirect_instructions(char *redir_str, t_cmdline *cmdline)
// {
// 	int				*redir_fd;
// 	unsigned char	escape_flag;
// 	char			*file_path_end;

// 	escape_flag = 0; //this is a subparser, so it has its own escape flag, but we interpret any unescaped quote as end of redirect because redirects *must* be unquoted to begin with
// 	//Condiciones de salida:
// 	//(!*redir_str || !escape_flag && (*redir_str == ';' || *redir_str == '|' || *redir_str == ' ' || *redir_str == '\'' || redir_str == '"') )
	
// 	//set instruction, get file name
// 	//wrongheaded... i need to know where file name ends before going in here... this needs to determine where INSTRUCTION SET ENDS...
// 	//echo "2*3" >> mult \; yoquese <
// 	//echo "2*3" >> mult ; yoquese
// 	//WHERE DOES THE INSTRUCTION SET END??? IT ENDS IF WE CAN'T FIND MORE INSTRUCTIONS BEFORE T HE END OF THE COMMAND LINE.
// 	//END OF CMDLINE DEFINED BY '\0', or UNESCAPED ';', '|'
// 	//INSTRUCTIONS CAN'T EXIST BETWEEN QUOTES
// 	while (*redir_str == '>' || *redir_str == '<') //So long as we keep finding new instructions...
// 	{
// 		//found instruction, now interpret instruction
// 		if (*redir_str == '<') //read from file, increment... maybe move the increment here and if the dereferenced char is null check fails and we end up evaluating the next condition too, but it can never succeed so it's ok... depends how desperate Norminette maks me
// 		{
// 			redir_fd = cmdline->fd_redir_in; //save fd of subsequent file name to memory pointed to by cmdline->fd_redir_in
// 			redir_str += 1; //increment pointer past the '<'
// 		}
// 		else if (*redir_str == '>') //write to file
// 		{
// 			redir_fd = cmdline->fd_redir_out; //save fd of subsequent file name to memory pointed to by cmdline->fd_redir_out
// 			if (*(redir_str + 1) == '>') //if it's the double ... eh... arrow thingy...
// 			{
// 				cmdline->redir_out_flag = 2; // set append mode
// 				redir_str += 2; //and increment pointer past the "<<"
// 			}
// 			else //if it's the single '>'
// 			{
// 				cmdline->redir_out_flag = 1; // set truncate mode
// 				redir_str += 1; //and increment the pointer past the '<'
// 			}
// 		}
// 		redir_str = ft_skipspaces(redir_str); //now skip any spaces, spaces are legit in redirect instruction world as long as there are more redirect instructions to find...
// 		if (!*redir_str || *redir_str == '>' || redir_str == '<' || *redir_str == ';' || *redir_str == '|' || *redir_str == '\'' || redir_str == '"') //if we find a null, another '<' or '>', ';', '|' or a quote because seriously... fuck that noise... it's a syntax error. :p
// 			return (print_error("micli: syntax error near unexpected token %c", *redir_str)); //return with synerror
// 		//file path starts here, get file path
// 		//WHAT DEFINES END OF FILE PATH???? NULL CHAR OR UNESCAPED SPACE, SEMICOLON, PIPE, 
// 			//get file path stuff (find end of file path)
// 			//if redir_fd pointer isn't NULL, close existing fd before reusing it, beacuse i'm mr clean
// 			//open/create file stuff
// 		//find next instruction, or die trying
// 		redir_str = ft_skipspaces(redir_str); //skip any spaces... we should find either another instruction, or something else
// 		//if we find another instruction, rinse and repeat
// 	}
// 	return (redir_str); //return end of detected instruction set	
// }

// char	*get_redir_fds(char *filename, t_micli *micli)
// {
	
// }


/*
** A redirect instruction token begins if an unescaped and unquoted '>' or '<'
** is found in the command line being analysed.
**
** Finding a redirect instruction will engage redirect parsing mode by executing
** the function retrieve_redirect_instructions to parse the line, retrieve the
** the file name paths to use and interpret the associated redirect instructions,
** save the associated in and out fds to cmdline level variables for use during
** command execution, and identify the end of the redirect instruction.
*/

// int		is_redirect_instruction(const char *chr, t_tokendata *tokdata, t_cmdline *cmdline)
// {
// 	if ((*chr == '>' || *chr == '<') && !tokdata->quote_flag && !tokdata->escape_flag)
// 		retrieve_redirect_instructions(chr, cmdline);
// 	return (0); //ARREGLA ESTO ANTES DE USARLO :p
// }