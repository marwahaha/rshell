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

