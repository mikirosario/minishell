# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    bugreport.md                                       :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miki \<miki@student.42.fr\>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/04/02 21:50:32 by miki              #+#    #+#              #
#    Updated: 2021/04/02 21:50:32 by miki             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# BUG REPORT

# CASO 1 - 🚀 NO ES UN BUG

export $var=test
bash devuelve "export: not valid in this context: /bint/test
el nuesto no devuelve nada

	RESPUESTA:
	
	En mi prueba tanto bash como minishell devolvieron lo mismo. En ambos casos
	he comprobado que el valor de retorno devuelto por echo $? es 1.
	
	BASH:
	bash$ export $var=test
	bash: export: `=test': not a valid identifier
	
	MINISHELL:
	🚀 export $var=test
	💥 export: '=test': not a valid identifier
	
## CONCLUSIÓN:
- La diferencia de mensajes de error probablemente se deba a diferencia
entre Mac y Linux. El que no devolviera nada minishell en tu prueba
podría ser porque var estaba definida. Pruébalo de nuevo haciendo primero
unset var.

# CASO 2 - 🚀 NO ES UN BUG

export "" test=a
bash devuelve "export: not valid in this context:"
el nuestro devuelve "export '': not a valid identifier"

	RESPUESTA:

	En mi prueba tanto bash como minishell devolvieron lo mismo. En ambos casos
	he comprobado que el valor de retorno devuelto por echo $? es 1.

	BASH:
	bash$ export "" test=a
	bash: export: `': not a valid identifier

	MINISHELL:
	🚀 export "" test=a
	💥 export: '': not a valid identifier

## CONCLUSIÓN
- La diferencia entre los mensajes de error se debe a una diferencia entre Mac
y Linux. No es un bug.

# CASO 3 - 🚀 NO ES UN BUG

echo ~
bash devuelve el directorio home
el nuestro hace escribe ~

	RESPUESTA:

	Efectivamente, bash parece resolver ~ a $HOME en los casos siguientes:

	El token comienza por una virgulilla sin más:
	bash$ echo ~
	/home/miki

	El token comienza por una virgulilla seguida por una barra:
	bash$ echo ~/
	/home/miki/

	Si le metes un espacio escapado delante de la virgulilla, bash no la
	resuelve:
	miki@Fenix2:~/C_Projects/minishell$ echo \ ~
	 ~

	Nuestro parser no resuelve la virgulilla a HOME (aunque nuestro cd sí que
	lo hace por su cuenta para facilitarnos la vida).

## CONCLUSIÓN
- It's not a bug, it's a feature? ;) El enunciado no pide que la virgulilla se
resuelva, por lo que no me parece un bug que sea así. Echo funciona bien, y el
parser según las indicaciones del enunciado. Se podría incorporar la
funcionalidad de resolver ~ (en cuyo caso habría que eliminarla de cd, porque
lo está haciendo por su cuenta y ya sería redundante), pero no es ni obligatorio
ni bonus.

# CASO 4 - 🚀 NO ES UN BUG

export var=te"st

bash espera con devolviendo "dquote\>"
el nuesto lo exporta sin mas

	RESPUESTA: Efectivamente, es así.

## CONCLUSIÓN
- It's not a bug it's a (missing) feature. El enunciado dice explícitamente que
no gestionemos comandos multilínea en la parte obligatoria. Por lo tanto, la
gestión del parser de minishell ante saltos de línea que dejan comillas abiertas
es de procesar la línea como si se hubieran cerrado al final. En este caso la
'traduce' de export var=te"st a export var=te"st". Todo eso es correcto. Si
metes export var=te"st" en bash hace lo mismo que minishell.

# CASO 5 - 🚀 NO SOLO NO ES UN BUG, EL BASH DE MAC ES EL QUE TIENE EL 'BUG' xD

.

bash devuelve ".: not enough arguments"
el nuestro ".: No such file or directory"

	RESPUESTA: Efectivamente, es así. Para ser más precisos, bash en Mac
	devuelve ".: not enough arguments". Mi bash en Ubuntu dice:

	bash$ .
	bash: .: filename argument required
	.: usage: . filename [arguments]

## CONCLUSIÓN
- No es un bug, es una diferencia de opinión sobre el problema. Tanto bash de
Ubuntu como minishell interpretan que el fallo se debe a que no existe un
archivo llamado ".". En Ubuntu es más fino... sabe que en realidad es imposible
que exista tal archivo, y que lo que pasa es que no has metido ningún filename.

Bash en Mac, en cambio, por algún motivo, cree que el problema es que no has
metido suficientes argumentos. O sea, que si tan solo dijeras ". ." el problema
se resuelve. Claaaaro que sí, guapi...

Bash en Mac es tonto del culo. Bash en Ubuntu y minishell tienen razón.

PD: Enséñale esto a Mario, le va a encantar. xD

# CASO 6 - 🚀 NO ES UN BUG

echo "12\"

bash espera devolviendo "dquote\>"
el nuestro 12"

	RESPUESTA: Efectivamente, es así.

## CONCLUSIÓN
- It's not a bug it's a (missing) feature. El enunciado dice explícitamente que
no gestionemos comandos multilínea en la parte obligatoria. Por lo tanto, la
gestión del parser de minishell ante saltos de línea que dejan comillas abiertas
es de procesar la línea como si se hubieran cerrado al final. En este caso la
'traduce' de "12\" a "12\"". Todo eso es correcto. Si "12\"" en bash hace lo
mismo que minishell.

# CASO 7 - 🚀 NO ES UN BUG

$LESS$VAR

bash no devuelve nada
el nuestro "micli: : command not found"

	RESPUESTA: Efectivamente, es así.

## CONCLUSIÓN
- Entiendo que si bash intenta resolver una variable pasada como comando y no lo
consigue, no se molesta ni en buscarlo. En cambio, micli resuelve la variable a
un string literal definido en minishell.h como null carácter (por motivos
relacionados con gestión de memoria en sus arrays), y lo busca, evidentemente no
encuentra nada, y por lo tanto devuelve command not found. No me parece bug,
para mí lo que hace minishell aquí también es válido, y aunque sea un pelín
menos eficiente que bash, también es más explícito para el usuario.

# CASO 8 - 💥 UN BUG!

..
bash "..:command not found"
el nuestro "..: No such file or directory"

	RESPUESTA: Efectivamente, es así. Resulta que había un comportamiento raro.
	Dos, en realidad, que han sido destapados gracias a este caso. :)

## CONCLUSIÓN
- El error diferente de por sí no era grave, pero era consecuencia de dos bugs
de fondo de mayor importancia.

Por un lado, el puntero exec_path, cuya memoria apuntada se debía liberar en
exec_child_process, nulificando el puntero, de hecho NO se nulificaba porque
exec_child_process solo recibía la dirección a la memoria y NO la dirección al
propio puntero. En consecuencia, el puntero original mantenía su dirección, y
la función print_not_found, que imprime el error "command not found" o "No such
file or directory" en función de si ese puntero es o no nulo, encontraba que el
puntero no era nulo e imprimía el error incorrecto. Esto se ha arreglado
haciendo que exec_child_process reciba un puntero al puntero exec_path en lugar
de una copia directa del puntero exec_path.

Por otro lado, al encontrar '..' como comando, micli iba a buscarlo y lo
encontraba en el primer directorio, claro... porque '..' es parte de la dir
list. O_O Por lo que intentaba ejectuar '..' y, por supuesto, fallaba. No era
grave porque fallaba en ejecución, pero me parecía muy irregular en todo
caso que lo hiciera, por lo que he modificado exec_cmd para que ya no busque
el comando en ningún sitio si es '..' o '.' sin más. Esto significa que de
ahora en adelante, no podemos tener builtins que se llamen '.' o '..'. ;)

Como efecto secundario de estos cambios, nuestro error ahora se imprime igual
que en bash.

# CASO 9 - 💥 UN BUG!

echo\ a
bash devuelve "command not found"

	RESPUESTA: Efectivamente, es así. No se escapaban los espacios.

## CONCLUSIÓN
- No se detectaban los espacios escapados ni los '\>' y '\<' escapados por un
error de parseo, pero esto ya se arregló en versión 4.412 y ya no ocurre.

# CASO 10 - 💥 UN BUG!

cat | cat | cat | ls
bash deja la terminal esperando
la nuestra la devuelve

	RESPUESTA: Efectivamente, es así. Y va a ser jodido de entender... :p

## CONCLUSIÓN
- La guarrada que inventé de contar hasta 1000000 después de arrancar cada hijo
y mirar justo después si había crasheado al final nos ha pasado factura, y la
factura ha sido este bug justo. xD El primer cat entra sin STDIN y sin archivo,
por lo que debería quedarse esperando, el segundo cat ídem, y el tercero ídem,
mientras que ls se ejecuta normal. Al enviar cualquier input al primer cat, sale
en cascada a todos los demás cat, y todos hacen exit. El comando ls, mientras,
al no esperar STDIN nunca porque no hace ningún read, hace tiempo que se ha
jubilado a una isla caribeña.

La idea del invento era que cada hijo muerto diera buena cuenta de su muerte y,
en función de esa cuenta, imprimiéramos los mensajes de error oportunos. Así, si
nos mandaban un path y salía mal, lo sabíamos al momento e imprimíamos el error.
Por otro lado, esperaba SOLO al último hijo al llegar al fin del pipeline,
entendiendo que los wait de broken_pipe_check se habían 'encargado' de los hijos.

Lo he cambiado. No solo provocaba este bug, sino que he visto que dejaba los
procesos hijos como zombies (sin cosechar al final de sus vidas). Ahora el
padre va abriendo hijos y cuando los tiene todos, los espera a TODOS antes de
continuar, como buen padre. Esto ha tenido el efecto de resolver el bug, y
evitar que los hijos queden zombies. He movido la impresión del error si no se
encuentra el path directamente al hijo, que al estar desviado STDOUT se imprime
a STDERR (AHHHH, para ESO servía xD).

Por desgracia, aún no me explico exactamente el por qué de este bug. Mi teoría
era que ocurría lo siguiente: En un pipe SIN errores, como cat | cat | ls, los
cat se quedaban esperando a STDIN, y el pipeline terminaba en un comando (como
ls) que NO espera a STDIN, con lo que el programa sale del pipeline y vuelve al
prompt, que escribe a STDOUT con un write, provocando el cierre en cascada de
todos los cat abiertos cuando leen el prompt de STDIN. Si en medio había
comandos 'rotos', al escribir sus respectos mensajes de error a STDOUT podían
provocar lo mismo respecto a otros procesos al enviar sus mensajes de error.

Pero esta teoría hace aguas. Una laguna es: cat | echo test. A pesar de que echo
envía datos a STDOUT, cat no los lee de STDIN. Se imprime test en pantalla, pero
cat se queda esperando. He hecho otras pruebas enviando datos a STDOUT desde el
padre y esperando que el hijo los recogiera por STDIN, y en ninguna el hijo se
ha descolgado.

A menos que haya alguna explicación especial detrás de de este comportamiento,
mi teoría no puede ser cierta, y me quedo sin saber qué provocaba el bug. :/

En cualquier caso ahora hago que el padre espere a todos los hijos al final del
pipe como un shell normal, y el error parece haberse curado.

# CASO 11 ? DISCUTIBLE

ls | exit
bash no hace nada
el nuestro sale

	RESPUESTA: Efectivamente, es así, pero vaya pijada... Aunque sé que se trata
	de imitar bash, zsh lo hace como micli. No hace excepciones en la ejecución
	de comandos de pipes con 'exit'. :p

## CONCLUSIÓN
- Podría modificar el comportamiento de exit en caso de que viera que se
encuentra en medio de un pipe, pero de momento voy a seguir... no me parece muy
prioritario al ser muy interpretable cuál es el comportamiento 'correcto' en ese
caso.

# CASO 12 ? DISCUTIBLE

sleep 5 | exit
bash esepra el sleep y luego no hace nada
el nuestro no hace el sleep y sale

	RESPUESTA: En realidad, el nuestro hace el sleep, y sale antes de que
	termine el sleep, pero con 'ps' ves que el sleep, ahora huérfano, sigue en
	el fondo hasta que acaban los cinco segundos. Nuevamente, este
	comportamiento no es el que elige bash, pero sí zsh.
## CONCLUSIÓN
- Básicamente es idéntico al Caso 11.

# CASO 13 ? DISCUTIBLE

\<a cat \<b \<c
bash devuelve "a: No such file or directory"
el nuestro "syntax error near unexpected token '\<a'
\> test | echo blabla; rm test
bash devuelve "blabla"
el nuestro "sysntax error near unexpected token '\> '
\>a cat \<b \>\>c
bash devuelve "b: no such file or directory"
el nuestor "syntax error near unexpected token '\>a'
\>a ls \>b \>\>c \>d
bash no devuelve nada
el nuestro "syntax error near unexpected token '\>a'
\>a ls \<machin \>\>c \>d
bash devuelve "machin: No such file or directory"
el nuestro "syntax error near unexpected token '\>a'
\>file
bash no devuelve nada
el nuestro "syntax error near unexpected token '\>f'

	RESPUESTA: Esto ocurre por un asunto más de fondo... prohíbo que un '\<'
	esté en la primera posición de una línea. Bash lo interpreta...

## CONCLUSIÓN
- Más que un bug, parte de una ingenuidad que tenía respecto al ordenamiento de
comandos que emplean redirección, presuponiendo que el comando siempre debía
estar al princpio. Técnicamente en ese aspecto no imita bash a la perfección,
pero modificarlo ahora sería delicado, costaría tiempo, y correría el riesgo de
introducir nuevos bugs...

# CASO 14 💥 UN BUG!

cat -e \> test1 \< test2
bash devuelve "test2: No such file or directory"
el nuestro no devuelve nada
cat \< test
bash no such file or directory
el nuestro no devuelve nada

	RESPUESTA: Efectivamente, es así. En este caso bash intenta usar test2 como
	input de cat antes de intentar crearlo, y aborta al encontrar el error.
	Micli tiene el comportamiento de crear todos los archivos si no existen ya.

- He quitado el O_CREAT flag del open para los archivos con permisos de lectura.
Además, he introducido un mensaje de error parecido al de bash en caso de fallar
el open. Si falla open, devuelve -1. Esto provocaba que cat se colgara esperando
STDIN al ejecutarse sin entrada de datos. Para evitarlo, he introducido una
condición especial en los hijos que ahora si reciben un fd de lectura de valor
-1 deben cerrar el fd de escritura y salir inmediatamente, devolviendo
EXIT_FAILURE. Es un poco ñapa, pero ahora imitamos bash.

# CASO 15 ? DISCUTIBLE


export var="  truc"; echo $var
bash devuelve "truc"
el nuestro "  truc"
export var="truc  "; echo $var | cat -e
bash devuelve "truc$"
el nuestr "truc  $"

	RESPUESTA: Efectivamente, es así. Increíblemente, en export sí guarda los
	valores con los espacios, y es bash el que decide quitarlos al resolver la
	variable. El zsh se comporta igual que micli en el mismo caso.

- Esa una diferencia de interpretación de lo correcto en la que, francamente, me
pongo totalmente de lado de zsh. Lo de 'interpretar' que el usuario metió unos
espacios que en realidad no quería me parece muy soberbio. En todo caso, es una
de esas cosas que diría... podría ajustar, pero no me parece que valga el tiempo
que le tendría que invertir, ni el riesgo de introducir nuevos bugs que entraña
todo cambio.

# CASO 16 🚀 NO ES UN BUG

$bla (non-export)
bash devuelve "syntax error near unexpected token 'non-export'
el nuestro " : command not found"
echo $var bonjour ($var non export)
bash devuelve "syntax error near unexpected token '('
el nuestro devuelve "bonjoun ( non export)

	RESPUESTA: En realidad micli devuelve, si var existe:
	test: bonjour: unexpected operator
	Y si no existe:
	micli: : command not found

- Tiene pinta de tratarse de alguna funcionalidad exótica de las que no vienen
en el enunciado. Pero micli gestiona estos casos con dignidad y de forma lógica.

# CASO 17 💥 UN BUG!

export test="file1 file2" ; >$test
bash devuelve "$test: ambiguos redirect"
el nuestr "micli: : command not found"

	RESPUESTA: El mensaje de error es lo de menos, pero esta prueba ha destapado
	un bugg gravísimos, que tiraba segmentation fault...

- Por un lado, con esta prueba me di cuenta de un fallo grave. Al intentar
resolver variables indefinidas, se trataban como variables definidas, provocando
segmentation fault al salir más allá del nulo en busca de su valor. Esto se ha
arreglado. Por otro lado, el parser considera que el  >$test es un comando e 
intenta buscarlo.
por eso devuelve command not found. En el segundo, parece saltarse el espacio.
Pero no lo hace siempre.

export "" test=a
bash devuelve "export: not valid in this context:"
el nuestro devuelve "export '': not a valid identifier"
echo ~
bash devuelve el directorio home
el nuestro hace escribe ~
export var=te"st
bash espera con devolviendo "dquote\>"
el nuesto lo exporta sin mas
.
bash devuelve ".: not enough arguments"
el nuestro ".: No such file or directory"
echo "12\"
bash espera devolviendo "dquote\>"
el nuestro 12"
$LESS$VAR
bash no devuelve nada
el nuestro "micli: : command not found"
..
bash "..:command not found"
el nuestro "..: No such file or directory"
echo\ a
bash devuelve "command not found"
cat | cat | cat | ls
bash deja la terminal esperando
la nuestra la devuelve
ls | exit
bash no hace nada
el nuestro sale
sleep 5 | exit
bash esepra el sleep y luego no hace nada
el nuestro no hace el sleep y sale
\<a cat \<b \<c
bash devuelve "a: No such file or directory"
el nuestro "syntax error near unexpected token '\<a'
\> test | echo blabla; rm test
bash devuelve "blabla"
el nuestro "sysntax error near unexpected token '\> '
\>a cat \<b \>\>c
bash devuelve "b: no such file or directory"
el nuestor "syntax error near unexpected token '\>a'
\>a ls \>b \>\>c \>d
bash no devuelve nada
el nuestro "syntax error near unexpected token '\>a'
\>a ls \<machin \>\>c \>d
bash devuelve "machin: No such file or directory"
el nuestro "syntax error near unexpected token '\>a'
\>file
bash no devuelve nada
el nuestro "syntax error near unexpected token '\>f'
cat -e \> test1 \< test2
bash devuelve "test2: No such file or directory"
el nuestro no devuelve nada
cat \< test
bash no such file or directory
el nuestro no devuelve nada
export var="  truc"; echo $var
bash devuelve "truc"
el nuestro "  truc"
export var="truc  "; echo $var | cat -e
bash devuelve "truc$"
el nuestr "truc $"
$bla (non-export)
bash devuelve "syntax error near unexpected token 'non-export'
el nuestro " : command not found"
echo $var bonjour ($var non export)
bash devuelve "syntax error near unexpected token '('
el nuestro devuelve "bonjoun ( non export)
export test="file1 file2" ; \>$test
bash devuelve "$test: ambiguos redirect"
el nuestr "micli: : command not found"
export test="file1 file2" ; \>"$test"
bash no devuelve nada
el nuestro "micli: : command not found"
echo bonjour \> $test
bash devuelve "$test: ambiguos redirect"
el nuestro no devuelve nada
echo bonjour \> $test w/ test="o1 o2"
bash "$test ambiguos redirect"
el nuestro nada
unset "" test
bash devuelve '': not a valid identifier
el nuestro nada
unset =
bash devuelve '=': not a valid identifier
el nuestro nada