<h1>rshell</h1>
-------------------

<h2>Introduction</h2> 
--------------------
In this rshell assignment we were tasked to create a program that mimics the abilties of a bash shell with linux commands.
Some of these commands include: mkdir, ls, echo and others.

<h2>Design</h2>
--------------------
We first started off by designing our program to work with bool functions to give us an easier time if we had a syntax error, since in our main function we run the code in a while loop until the user inputs "exit". We then removed comments but gave leeway if it was contained in a quotation mark. We then implemented multiple test cases and other ways of sorting syntax errors out before we used a 4-Case system before running any actual executing code. We used a single command case, case only involving a semicolon, a case involving only connectors (&& and ||), and a case involving both semicolons and connectors. 

<h2>Bugs</h2>
----------------------
Some known bugs of our program are when the user enters in something like ls ;     ;, this will not be caught by our test cases as we could not figure out a way to deal with the spaces inbetween quotes without messing up the code (e.g ls -a; ls -a;) would be throw into a syntax error.

We also could not figure a way to properly throw a syntax error when (ls && ) happens since it would break the code for (ls && ls). 

<h1>Part 2</h1>
-------------------
In the second part of our assignment we were tasked with updating our rshell program so that it also could identify if the correct file path as well as being able to handle priroity with parentheses. 

<h2> Design </h2>
--------------------
We first started off handling paraentheses, and decided to incorporate a new bool function before it was handled to our parse_string function. The initial string it essentially stripped of the paraentheses and correctly identifies the ordering and runs the commands in the correct order. We then handled the flags and pathnames in which we used a BOOST to first seperate out the command then ran multiple checks to see which kind of flag the user specified before processing if it was the correct path.

<h2> Bugs </h2> 
--------------------
If we do not close the parentheses our program does not behave as the normal terminal would as any inputs that still come in are still recognized by the terminal until the brace is closed.

<h1> Part 3</h1>
-------------------
In the third part of our assignment we were tasked with updating our rshell program to handle input and output redirection as well as piping.

<h2> Design </h2>
-------------------
We decided to handle this in a simple if statemnet if the code decected <, >, >> or | at all it would then go into a run_cmd function that was responsible for handling piping first then threw the code to inputoutput function to handle redirections. We needed a helperpiping function to find the count of pipes as well as a flag function which was tasked to indentify if it was <, > or >> then assigned a bit-manipulated value to a int which was then used in our inputoutput function.

<h2> Bugs</h2>
-------------------
A potential bug that we are not sure if we will be tested on is handling it with the addition of the first parts of the program. For example, if the code (ls > input || echo a) && echo b) is entered the code will do all three commands. We could not figure a way to implement so that it can handle everything since we handled piping and everything in a true-false if statement before parsing the string.
