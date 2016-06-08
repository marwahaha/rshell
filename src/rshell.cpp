#include <cstddef>
#include <fcntl.h>

#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;
// handles input and output redirection

int globalvar = -1; 

string inputoutput (mode_t *tt , vector<string> *io, string a , int mode,int *flag)
{
    string bad;
    // if mode is equal to sign set flag to one and return nothing
	if (mode == 0)
		{
			*flag = 1;
			return bad;
		}
	// if statement only deals when the mode is 1 (explanation at the mode func)
	// basic input
	if((mode&1)>=1)
		{
		string b; 
		int d = a.find_last_of('<'); 
		int e =0 ;
		b = a.substr(a.find_last_of('<'));
		if (b.find_first_not_of("< ") == string::npos)
		{
			*flag = 1 ; 
			cout << "missing file name"<< endl; 
			return bad;
		} 
		b.find_first_not_of("< "); 
		b =b.substr(b.find_first_not_of("< "));
		if ((b.find(">") != string::npos) || b.find(' ') != string::npos)
        {
			b= b.substr(0 ,b.find_first_of(" >")); 
			e = a.find(b)- d;  
        }
		if (e!=0)
		a.erase(d,b.size()+e); 
		else
		a.erase(d); 
		io->at(0) = b; 	
	}
	// deals when mode is 4 
	// aka basic output redirection
	if((mode&4) >0)
	{
		io->at(3) = "0"; 
		if (isdigit(a.at(a.find_first_of('>')-1)))
		{
			io->at(3) = a.at(a.find_first_of('>')-1);
			a.at(a.find_first_of('>')-1) = ' ' ; 
		}
		string b; 
		b = a.substr(a.find_last_of('>'));
	int d = a.find_last_of('>'); 
	int e = 0; 
		if (b.find_first_not_of("> ") == string::npos)
		{
			*flag = 1; 
			cout << "missing file name"<< endl; 
			return bad;
		}
		b =b.substr(b.find_first_not_of("> "));
		if(b.find(' ') != string::npos || (b.find('<') != string::npos))
{
			b= b.substr(0 ,b.find_first_of(" <"));	
			e= a.find(b)- d; 
}
		if (e!=0) 
		a.erase(d,b.size()+e); 
		else 
		a.erase(d); 
		io->at(1) = b; 
		// essentially opens the file for writing too
		*tt = (O_WRONLY|O_CREAT|O_TRUNC); 
	}
	// deals when mode is 8
	// double output
	else if((mode&8) >0)
	{
		io->at(3) = "0"; 
		if (isdigit(a.at(a.find_first_of('>')-1)))
		{
			io->at(3) = a.at(a.find_first_of('>')-1);
			a.at(a.find_first_of('>')-1) = ' ' ; 
		}
		string b; 
		b = a.substr(a.find_last_of('>'));
		int d = a.find_last_of('>')-1; 
		int e =0 ;
		if (b.find_first_not_of("> ") == string::npos)
		{
			*flag = 1 ;
			cout << "missing file name"<< endl; 
			return bad;
		}
		b =b.substr(b.find_first_not_of("> "));
		if(b.find(' ') != string::npos || (b.find('<') != string::npos))
        {
			b= b.substr(0 ,b.find_first_of(" <"));	
			e = a.find(b)-d; 
        }
		if (e!=0) 
		a.erase(d,b.size()+e); 
		else 
		a.erase(d); 

		io->at(1) = b; 
		*tt = (O_WRONLY|O_CREAT |O_APPEND); 
	}
	
	return a; 	
}

bool flag_checker(string command) {
    try
    {
        // we have to again split the string up into parts to deal with
        // the commands
        vector<string> flagcommands;
        string flags, pathnames;
        char_separator<char> sepspaces(" ");
        tokenizer<char_separator<char> > token_spaces(command,sepspaces);
        BOOST_FOREACH(string t, token_spaces) {
            flagcommands.push_back(t);
        }
        // now that we have parsed the string
        // we can go into the different test cases
        // First  we can just start with the most basic test case 
        if(flagcommands.at(0) == "test" && (flagcommands.at(1) == "-e"
         || flagcommands.at(1) == "-f"
        || flagcommands.at(1) == "-d")) {
            flags = flagcommands.at(1);
            pathnames = flagcommands.at(2);
        }
        else if (flagcommands.at(0) == "test" &&
        flagcommands.size() == 2) {
            //this makes it so that we always run -e as default
            flags = "-e";
            pathnames = flagcommands.at(1);
        }
        // now we can check for if brackets are used
        else if (flagcommands.at(0) == "[") {
            bool missingendbrace = false;
            if (flagcommands.at(2) == "]" || flagcommands.at(3) == "]") {
                missingendbrace = true;
                if (missingendbrace == false) {
                    return false; // Error since no missing end brace
                }
            }
            // Now we can deal with the normal test cases
            if (flagcommands.at(1) == "-e" || flagcommands.at(1) == "-f" 
            || flagcommands.at(1) == "-d") {
                flags = flagcommands.at(1);
                pathnames = flagcommands.at(2);
            }
            // Now we can set the default case
            else {
                flags = "-e";
                pathnames = flagcommands.at(2);
            }
        }
        else {
            return false;
        }
        struct stat* c1 = new struct stat;                  
        //DECLARE a new stat struct for use with stat function
            char * path =  new char [pathnames.size()+1];
            for(unsigned i=0; i < pathnames.size();i++)          
            //converts the string to char* 
            path[i]=pathnames.at(i);
            path[pathnames.size()]='\0';                         
            //last element needs to be null to prevent seg faults
            //what this essentially does is takes the path and c1 and
            //runs it thro stat function which is essentially a return
            //of an error value for runtime failure
            int returnval = stat(path, c1);                    
            if(returnval == -1){                              
                cout<<"(False)"<<endl;                         
                return false;
            }
            
            int isdir = (c1->st_mode & S_IFDIR);                
            //checking if flags for S_IFDIR and S_IFREG are set
            int isreg = (c1->st_mode & S_IFREG);               
            
            if(flags=="-e")                                      
            //different cases for inferring the type of file.
            {
                cout<<"(True)"<<endl;
                return true;
            }
            else if(flags=="-f" && isreg!=0)
            {
                cout<<"(True)"<<endl;
                return true;
            }
            else if(flags=="-d" && isdir!=0)
            {
                cout<<"(True)"<<endl;
                return true;
            }
            else
            {
                cout<<"(False)"<<endl;
                return false;
            }
            return false;
        }
        // implemented a method to catch improper function declarations 
        catch(std::exception e)                             
        {                                                  
            cout<<"-rshell: invalid test command syntax"<<endl;
            return false;
        }
}

int helperpipe(string a)
{
	int count = 0 ;
	while (1)
	{
		if (a.find('|') != string::npos)
		{
			count ++;	
			if(a.at(a.find('|')) == a.at(a.find_last_not_of(' '))) 
				return -1; 
			a= a.substr(a.find('|')+1);
		}
		else
			break; 
	}
	return count; 
}
//function in charge of actually executing the code
bool execute(char* a, int &t)   
//This function executes the command by accetping a c-string
{                               
    string commandlinestartup,argumentative;           
    //string for accepting 'COMMAND' and 'ARGUMENTS'
    int status;                 
    unsigned size = sizeof(a)/sizeof(char);   
    if(size == 0)                             // if size is 0 then quit 
        return false;
    
    
    //flag to make sure only the 'COMMAND' 
    //part of the program goes into commandlinestartup    
    bool flag = false;                        
    for(unsigned i = 0; i < size || a[i] != '\0'; i++)
    //converting a to string type variables
    //commandlinestartup and argumentative
    {
        //this snipped makes sure that only the COMMAND 
        //goes into the commandlinestartup part of the program
        if(a[i] == ' ' && !flag)  
        {
            flag = true; continue; 
            //once it encounters a space, 
            //the rest of the command goes into arugumentative
        }
        if(flag)
        {
            argumentative = argumentative + a[i];    
            continue;
        }
        commandlinestartup = commandlinestartup + a[i]; 
    }
    unsigned commandlinestartup_length = commandlinestartup.length() + 1; 
    // converting the cstring and breaking it down 
    char * command = new char [commandlinestartup_length];    
    // we have to do this since we need to get the length
    command[commandlinestartup.length()] = '\0';        
    //the last character is NULL since cstring needs a null 
    unsigned argumentative_length = argumentative.length()+1;   
    char * arguments = new char [argumentative_length];
    arguments[argumentative.length()] = '\0'; // same as above here
    for(unsigned i = 0; i< commandlinestartup.length(); i++)  
    // using these for loops we can translate the one cstring
    // into two distinct c-strings one in charge of command the
    // other deals with the arguments  
    {                                       
        command[i] = commandlinestartup.at(i);         
    }
    for(unsigned i = 0; i < argumentative.length(); i++)
    {
        arguments[i] = argumentative.at(i);
    }
    string commandtest = command;
    string argument = arguments;
    if(commandtest=="test" || commandtest=="[")
    {
        return flag_checker(a);
    }
    char* argv[3];  
    //the argv pointer array goes into the execvp function
    //, which is described inthe main
    if(argumentative.length() == 0)                
    //pages as follows: execvp(char* argv[0], argv )
    {
        argv[0] = command;       
        //single word commands do not have arguments so the others are null
        argv[1] = NULL;        
        argv[2] = NULL;
    }
    else
    {
        argv[0] = command;       // multiword commands
        argv[1] = arguments;
        argv[2] = NULL;
    }
    
    pid_t c_pid, pid;
    c_pid = fork();
    
    if(c_pid == -1)                       //failed fork
    {
        cout << "fork failed: please try again" << endl;
        perror("FORK:");
        return false;
    }
    else if(c_pid == 0)
    {
        t = execvp(argv[0],argv);
        printf("-rshell: %s: command not found\n", command);
        exit(t);
    }    
    else
    {
        pid = wait(&status);
        if(pid){}
        if(status == 0)
            return true;
        else 
            return false;
    }
}
// Point of this function is to help 
//convert a string to a C-string to pass to the execute function
bool simple_parse_string(string text) {
        string singlecommand; 
        char_separator<char> removespaces(" "); // takes out the " " 
        tokenizer<char_separator<char> > token_s(text, removespaces); 
        // create a tokenizer 
        bool first_token_added=false;
        BOOST_FOREACH(string singlea, token_s)             
        // BOOST iterates over token_s to find all the strings
        {                                                  
            if(!first_token_added)
            {
                first_token_added = true;                      
                singlecommand = singlecommand + singlea;              
                // every iteration add onto the existing singlecommand             
            }                                               
            else
                singlecommand = singlecommand + " " + singlea;      
                // makes sure the first passed in
                // string does not have a space
                // to avoid potential errors
        }
        // test case in case of "Exit"
        if (singlecommand == "exit") {
            return true;
        }
        // another test case in case of empty string
        if (singlecommand.length() == 0) {
            return true; 
        }
        char* commandtorun = new char[singlecommand.size() + 1]; 
        //prepares the single to pass to execute function
        memcpy(commandtorun, singlecommand.c_str(), singlecommand.size() + 1);
        int m;
        
        return execute(commandtorun, m);
}
// Handles finding if < > or >> exists
// we have to use bit manipulation here so our input output
// function works
int findthesign ( string a)
{
	int signflags =0 ; 
	if (a.find('<') == string::npos &&  a.find('>') == string::npos) {
		return signflags; 	
	} 
	if( a.find('<') != string::npos && 
	(a.find_last_of('<') == a.find_first_of('<'))) {
		signflags  |= 1; 
	}  // looks for an output then tries to find if its onthe only output
	// since last and first are equal
	else if (a.find('<') !=string::npos) {
		signflags = -1;
	} // subtract -1
	if (a.find('>') != string::npos && 
	(a.find_last_of('>') == a.find_first_of('>'))) {
		signflags |= 4; 
	} // looks for only one input
	else if (a.at(a.find_first_of('>')+1) == '>' && 
	(a.find_last_of('>') == a.find_first_of('>')+1)) {	
		signflags |=8;
	} //looks for a double output redirection
	else if (a.find('>') != string::npos) {
		signflags = -1; 
	} // subract -1 flag
	return signflags; 
}

int run_cmd(string nocomments) 
{
    int mode = findthesign(nocomments);
	vector <string > pipes ;  
	int pipecount = helperpipe(nocomments); 
	// if we have negative pipe throw and error and restart
	if (pipecount < 0)
	{
		cout << "error" << endl;
		return true; 
	}
	string backup = nocomments; 
	int curcount = 0; 
	string stringbackup = nocomments;
	vector<string> pipecmd; 
	//If the pipe count is > 0 then 
	// find the last | and substr it into a string stringbackup
	// for every counter of pipecount 
	// pushback into our vector of pipecmd the split of where the | is.
	if (pipecount> 0 )
	{
		stringbackup = nocomments.substr(nocomments.find_last_of('|')+1);
		string tempo = nocomments; 
		for (int j = 0 ; j <=pipecount ; j++)
		{
			pipecmd.push_back(tempo.substr(0, tempo.find('|')-1)); 		
			tempo = tempo.substr(tempo.find('|')+1); 
		}
	}
	int fd_in = 0;
	int pd[2]; 
    for (int z = 0; z != pipecount+1; z++)
    {

    	if (pipecount > 0) 
    	{
    			int err= pipe(pd); 
    			if (err<0) 	
    			{
    				perror ("pipe");
    				return true; 
    			}
    		nocomments = pipecmd.at(z); 
    	}
    
    	int pid = fork();
    	globalvar= pid; 
    	// IMPORTANT:
    	// Finds the correct bit manipulation and puts it into int mode
        mode = findthesign(nocomments);
        // checks pid for errors
    	if (pid > 0)
    	{
    
    		int err = wait(0); 
    		if (err<0)
    			perror("Wait"); 
    		if (pipecount>=1)
    		{
    			int err2 = close(pd[1]); 
    			if (err2< 0)
    				perror("Close");
    			fd_in = pd[0]; 
    			curcount++; 
    		}
    		
    	}
    	if (pid == 0)
    	{
    		if (pipecount > 0 )
    		{
    			int err2 = dup2(fd_in, 0); 	
    			if (err2<0)
    				perror("Dup2"); 
    			if ( z != pipecount)
    			{
    				int err3 = dup2(pd[1],1);
    				if (err3< 0 )
    					perror("Dup2"); 
    			}
    			int err3=close(pd[0]);
    			if (err3< 0)
    				perror("Close");
    		}
    
    		if (nocomments.find_first_of("<>")==string::npos)
    			mode = 0; 
    		if (mode!=0) {
    		    // This is the meat of the piping / redirection program
    		    // We give read and write permission
    		    // Next we have to make a vector 
    		    //of string that holds the bit 
    		    // maniplulation. We then throw no comments as well as 
    		    // the mode that we found from
    		    // findthesign function above. 
    		    
    			int placeholder = 0;
    
    			mode_t permission  ; 
    			mode_t permis = S_IRUSR |S_IWUSR;
    			vector <string> io; 
    			io.resize(5); 
    			int *flag = &placeholder; 
    			string a = 	inputoutput(&permission, &io, nocomments,
    			 mode, flag);
    
    		if (!io.at(0).empty() )
    		{
    			// Basically the new few if statements
    			// Simply check for errors using both dup and close
    			int fdi=open(const_cast <char*>(io.at(0).c_str()),O_RDONLY);	
    			if (fdi<0) {
    				perror("Read Error"); 
    				*flag = 1; 
    				return 1; 
    			}
    			int erro4 = close(0);
    				
    			int erro5 = dup(fdi);
    			if (erro4<0)
    				perror ("close");
    			if(erro5< 0 )
    				perror("dup"); 
    
    		}
    		if (!io.at(1).empty() )
    		{
    			int fdo=open(const_cast <char*>(io.at(1).c_str()),permission, permis);	
    			if (fdo<0) {
    				perror("writeError"); 
    				*flag = 1; 
    				return 1; 
    			}
    			int erro6; 
    			int erro7; 
    			if (io.at(3)== "0" )
    			{
    				erro6 = close(1);
    				erro7 =dup(fdo); 
    				if (erro6 < 0 )
    				perror("Close"); 
    			if (erro7 < 0 )
    				perror("Dup"); 
    
    
    			}
    			else
    			{
    				erro6 = close( atoi(const_cast<char*>(io.at(3).c_str()))); 	
    				erro7 = dup(fdo); 
    			}
    			if (erro6 < 0 )
    				perror("Close"); 
    			if (erro7 < 0 )
    				perror("Dup"); 
    
    		}
    		if(!io.at(2).empty())
    		{
    			char* yolo = new char(io.at(2).size()); 
    			strcpy(yolo, const_cast<char*>(io.at(2).c_str()));  
    			cout << "Need to do" << endl; 
    		}
    			if (*flag==1) {
    				cout << "Error" << endl; 
    				continue; 
    			}
    			nocomments = a;
    		}
    		vector< string> subcmd; 
    
    		char *ccstring = new char[nocomments.size()+1]; 
    		strcpy(ccstring, nocomments.c_str()); 
    
    
    		char *token = strtok(ccstring, " ");
    		while (token!=NULL)
    		{
    			subcmd.push_back(token);
    			token = strtok (NULL, " "); 
    		}
    		unsigned int argvsize= subcmd.size()+1; 
    		char **argv;
    		argv = new char*[argvsize]; 
    		for (unsigned int j = 0 ; j < subcmd.size() ; j++)
    		{
    			argv[j] = new char[subcmd.at(j).size()+1];
    			strcpy (argv[j],subcmd.at(j).c_str());  
    		}
    		argv[subcmd.size()] = 0; 
    
    			if (nocomments.find_first_not_of(" ") == string::npos && nocomments.size() >=1)
    			{	
    				cout << "Syntax Error "<< endl; 
    				exit(1); 
    			}
    			//getenv searches for the enviornemnt list 
    			// returns pointer to c string if matches
    			string path = getenv("PATH"); 
    			if(path.empty())
    				perror("getenv");
    			vector<string>paths;
    			while(1)
    			{
    				if(path.find_first_of(":") == string::npos)
    					break;
    				paths.push_back(path.substr(0,path.find(":")));
    				path = path.substr(path.find(":")+1);
    			}
    	
    			int acc = 0; 
    			for(unsigned i = 0 ; i < paths.size(); i++)
    			{
    
    				string temp = argv[0]; 
    				paths.at(i) += "/" + temp; 
    
    					if ((acc =  access(const_cast<char*> 
    					(paths.at(i).c_str()) , X_OK)) == 0)
    					{
    						execv( const_cast<char*> 
    						(paths.at(i).c_str()), argv); 
    						perror("execv"); 
    					}
    			}
    			if (acc< 0)
    				perror("access"); 	
    			exit(1); 
    		}
    
    	if (pid == -1)
    	{
    		perror("fork); exit(1)"); 
    	}
    	
    }
	
	return 0; 

}
bool parse_string(string t) {
    
    if (t.length() == 0) { // deals with empty string
        return true;
    }
    string nocomments; // create a string w/o comments
    char * commandtorun; // cstring ready to run the commands
    bool errorchecker = false; // deals with empty strings
    //fixing commenting and quote marks errors
    if (t.find('"') == string::npos) { 
		// if there exists a quote mark do NOT 
    //count the # 
        t = t.substr(0, t.find("#")); // shorten down the command line by 
    //removing the #
    }
    nocomments = t;
    nocomments.erase(remove(nocomments.begin(), nocomments.end(), '"'), 
    nocomments.end()); // removes all quotation marks from nocomments
    string temp;
    string newtemp;
    string semitemp;
    for (unsigned int i = 0; i < nocomments.size() - 1; i++) {
        if (nocomments.at(i) == ';' && nocomments.at(i + 1) == ';') {
            cout << "Syntax Error!" << endl;
            return true;
        }
    }
    //deals with wrong syntax
    for (unsigned int i = 0; i < nocomments.size(); i++) { 
        // used to find any extra repeating semi colons
        if (nocomments.at(i) == '&' && nocomments.at(i+1) == '|') {
            cout << "Syntax Error!" << endl;
            return true;
        }
    }
    
    // Deals with wrong syntax
    for (unsigned int i = 0; i < nocomments.size(); i++) { 
        // used to find any extra repeating semi colons
            if (nocomments.at(i) == '|' && nocomments.at(i+1) == '&') {
                cout << "Syntax Error!" << endl;
                return true;
            }
    }
    bool semi = (nocomments.find(";") == string::npos);
    bool aand = (nocomments.find("&&") == string::npos);
    bool oor = (nocomments.find("||") == string::npos);
    bool inputred = (nocomments.find("<") == string::npos);
    bool pipez = (nocomments.find("|") == string::npos);
    bool outputred1 = (nocomments.find(">") == string::npos);
    bool outputred2 = (nocomments.find(">>") == string::npos);
    vector <string> cmd; // dealing with semi colons
    char* cstring = new char[nocomments.size() + 1];
    strcpy(cstring,nocomments.c_str());

    if (inputred == false || pipez == false || outputred1 == false || outputred2 == false) {
        run_cmd(nocomments);
    }
    else {
        vector <string> subcmd; 
    // CASE 4 used to handle connectors inside semi colon command
    vector <string> subsubcmd; 
    // used to handle normal commands (no connectors)
    if (semi && aand && oor) { //handles the test case of a single command
        string singlecommand; 
        char_separator<char> removespaces(" "); // takes out the " " 
        tokenizer<char_separator<char> > token_s(nocomments, removespaces); 
        // create a tokenizer 
        bool first_token_added=false;
        BOOST_FOREACH(string singlea, token_s)             
        // BOOST iterates over token_s to find all the strings
        {                                                  
            if(!first_token_added)
            {
                first_token_added=true;                      
                singlecommand=singlecommand+singlea;              
                // every iteration add onto the existing singlecommand             
            }                                               
            else
                singlecommand=singlecommand + " " + singlea;      
                // makes sure the first passed in string does not have a space
                // to avoid potential errors
        }
        // test case in case of "Exit"
        if (singlecommand == "exit") {
            return true;
        }
        // another test case in case of empty string
        if (singlecommand.length() == 0) {
            return true; 
        }
        commandtorun = new char[singlecommand.size() + 1]; 
        //prepares the single to pass to execute function
        memcpy(commandtorun, singlecommand.c_str(), singlecommand.size() + 1); 
        // helps with memory leak
        int m;
        
        return execute(commandtorun, m);
        
    }
        // CASE 2
        // Dealing with multiple commands chained together with semi
        // SINCE NOT USING BOOST WE MUST EXECUTE IT DIFFERENTLY

    else if (!semi && aand && oor) {
        
        char *token = strtok(cstring, ";"); 
        // Had difficulty manipulating semicolons using BOOST decided to make 
        // a vector of strings to store the commands in
        if (token == NULL) {
            errorchecker = true;
        }
        while(token != NULL) {
            cmd.push_back(token);           
            // pushes back all the seperated commands into a vector
            token = strtok(NULL, ";");
        }
        
        if (errorchecker == true) {
            cout << "Syntax Error!" << endl;
        }
        if (errorchecker == false) {
            int counter = cmd.size();
            for (int i = 0; i < counter; i++) { 
                // HAD MUCH TROULBE IMPLTENTING A BOOST 
            simple_parse_string(cmd.at(i));     
            // Throwing it into simple_parse_string 
            //individual after seperating proved easier
            }
        }
        
        cmd.clear();
    }
         // testing to make sure everything is stored inside vector CMD
    
    // CASE 3
    // Dealing only with multiple commands 
    //chained together with the connects && and ||

    else if (semi && (!aand || !oor)) {
       
        vector<char> connector; // identify where all the && or || are present 

        for(unsigned i=0;i<nocomments.length()-1;)        
        {
            if(nocomments.at(i)=='&' && nocomments.at(i+1)=='&')  
            // this if looks at if there are two &&s together
            {
                if(nocomments.at(i+2)=='&' || nocomments.at(i+2)=='|') {
                    // another test case check 
                    cout << "Syntax Error" << endl;
                    return true;
                }
                connector.push_back('&'); 
                // then push back one & into connector
                i=i+2;
                continue;
            }
            if(nocomments.at(i)=='|' && nocomments.at(i+1)=='|')  
            // same just with |'s
            {                
                if(nocomments.at(i+2)=='&' || nocomments.at(i+2)=='|') {
                    cout << "Syntax Error" << endl;
                    return true;
                }
                connector.push_back('|');                  
                i=i+2;  
                continue;
            }
            else{
                i++;
            }
        }
        
        connector.push_back('\0'); // pushback to signal endof the vector
        bool aandandoorchecker = true;
        bool aflag=true;                                    
                                                           
        char_separator<char> sepnotamp("&|");                              
        tokenizer<char_separator<char> > token_notamp(nocomments,sepnotamp);  
        unsigned i=0;
        BOOST_FOREACH(string t, token_notamp) 
        // Using BOOST to split up by & and |
        {
            if(i>connector.size()-1)                       
                break;
            if(aflag)                   // always true on the first run
            {
                aandandoorchecker=simple_parse_string(t);
                aflag=false; // then turns false
                continue;
            }
            if(connector.at(i)=='&' && aandandoorchecker==true)         
            // if aandandoorchecker is true and the conncetor is &
            {                                                       
                // run the string
                aandandoorchecker=simple_parse_string(t);
                i++;
            }
            else if(connector.at(i)=='|' && aandandoorchecker==false)  
            //similar to above but aandanoorchecker MUST be false
            {
                aandandoorchecker=simple_parse_string(t);
                i++;
            }
            else                                       
            {                                          
                i++;
                continue;   
            }
            
        }
        return aandandoorchecker;
    }
                

    
    
    //CASE 4
    // Dealing with both commands chained together with semis and connectors 
    // implementing the same idea as just semicolons
    //first by removing all semicolons then dealing with
    // connectors

    else if( (!aand || !oor) && !semi )                   
    {                     
        string temp;
        for (unsigned int i = 0; i < nocomments.size(); i++) { 
            // used to find any extra repeating semi colons
            if (nocomments.at(i) == ';') {
                temp = nocomments.substr(i + 1, nocomments.length() - 1);
                break;
            }
        }
        for (unsigned int i = 0; i < temp.size(); i++) {
            if (temp.at(i) == ';') {
                cout << "Syntax Error!" << endl;
                return true;
            }
        }
        char *token = strtok(cstring, ";"); 
        // Had difficulty manipulating semicolons using BOOST decided to make 
        // a vector of strings to store the commands in
        if (token == NULL) {
            errorchecker = true;
        }
        while(token != NULL) {
            cmd.push_back(token);           
            // pushes back all the seperated commands into a vector
            token = strtok(NULL, ";");
        }
        
        if (errorchecker == true) {
            cout << "Syntax Error!" << endl;
        }
        
        
        for (unsigned int i = 0; i < cmd.size(); i++) {
            if ((cmd.at(i).find("&&") != string::npos) || 
            (cmd.at(i).find("||") != string::npos)) {
                subcmd.push_back(cmd.at(i)); 
                // if contains a condition pushes back into vector
                subsubcmd.push_back("FALSE");
            }
            else {
                subsubcmd.push_back(cmd.at(i)); 
                // push backs the normal command 
                //into a different vector<string>
                subcmd.push_back("FALSE");
            }
        } // devise a way to separate the next condition IF it has && or ||
        //figure out a way to chronologically evaluate both subcmd or subsubcmd
        // by push backing the keyword FALSE we can 
        //differentiate what order the code should be in
        // since only either subsubcmd or subcmd can be FALSE at one time 
        subcmd.resize(cmd.size()); // SUBCMD handles ;
        subsubcmd.resize(cmd.size()); // SUBSUBCMD handles && and ||
        for (unsigned int i = 0; i < cmd.size(); i++) { 
            // in this code we use both codes from CASE 2 and CASE 3
            if (subsubcmd.at(i).find("FALSE") == string::npos) {
                // What this if statement does is run similar code 
                // to dealing with what happens in the case
                // of a semicolon
                string letsrunthis = subsubcmd.at(i);
                simple_parse_string(letsrunthis);
                
            }
            else if (subcmd.at(i).find("FALSE") == string::npos) {
                // if the semicolon is not valid then it treats the 
                // code similar to how we handle && and ||s
                
                string figureitout = subcmd.at(i);
                vector<char> connector;
        
                for(unsigned z=0;z<figureitout.length()-1;)        
                {
                    if(figureitout.at(z)=='&' && figureitout.at(z+1)=='&')  
                    // If we can identify && and ||s 
                    // run against possible test case
                    // then do similar to Case 2 and push back into connector
                    {
                        if(nocomments.at(i+2)=='&' 
                        || nocomments.at(i+2)=='|') {
                            cout << "Syntax Error" << endl;
                            return true;
                        }
                        connector.push_back('&');
                        z=z+2;
                        continue;
                    }
                    if(figureitout.at(z)=='|' && figureitout.at(z+1)=='|')  
                    {       
                        if(nocomments.at(i+2)=='&' 
                        || nocomments.at(i+2)=='|') {
                            cout << "Syntax Error" << endl;
                            return true;
                        }
                        connector.push_back('|');                  
                        z=z+2;                                      
                        continue;
                    }
                    else{
                        z++;
                    }
                    
               }
                
                connector.push_back('\0'); // signals the end of connector
                bool aandandoorchecker = true;
                bool aflag=true;                                    
                                                                   
                char_separator<char> sepnotamp("&|");                              
                tokenizer<char_separator<char> > token_notamp(subcmd.at(i),
                sepnotamp);  
                unsigned b=0;
                BOOST_FOREACH(string t, token_notamp) // use boost to 
                // split the & and |s
                {
                    if(b>connector.size()-1)                       
                        break;
                    if(aflag)  // always true on the first run        
                    {
                        aandandoorchecker=simple_parse_string(t);
                        aflag=false;
                        continue;
                    }
                    if(connector.at(b)=='&' && aandandoorchecker==true)         
                    { // if aandandoorchecker is true and the conncetor is &
                    // run simple_parse_string
                        aandandoorchecker=simple_parse_string(t);
                        b++;
                    }
                    else if(connector.at(b)=='|' && aandandoorchecker==false)  
                    {  //similar to above but aandanoorchecker MUST be false
                        aandandoorchecker=simple_parse_string(t);
                        b++;
                    }
                    else                                       
                    {                                          
                        b++;
                        continue;   
                    }
                    
                }
                
            }
            
                
            }
            

    }
    }
return false;

}

bool checkbrackets (string t) {     
    int pcounter = 0;
    int begofstring = 0;
    int endofstring = 0;
    bool pexist = false; // bool dealing with p's exist
    bool endreached = false;
    bool pchecker = false; // bool dealing with if we have checked for p's
    bool nosecondpart = false;
    
    // we need to create a way to check for p's
    // First case: no p's
    for (unsigned i = 0; i < t.size(); i++) 
    {
        // we need to check to see if P's exist
        // a smart way to handle this is that we know p's 
        // will always be first in a command
        // ex ((echo a || echo c) && echo d)
        if (pchecker == false && pexist == false) {
            if (t.at(i) == '&' && t.at(i + 1) == '&') {
               pchecker = true;
               endofstring = i;
            }
            if (t.at(i) == '|' && t.at(i + 1) == '|') {
               pchecker = true;
               endofstring = i;
            }
        }
        // after we perform the check for p's
        // if we meet && or || first 
        // obviously p's do not exist
        if (pchecker == false && t.at(i) == '(')
        {
            if (pcounter == 0)
            {
                begofstring = i + 1;
            }
            pexist = true; // so we find p's first 
            pcounter++;
        }
        if (pchecker == false && t.at(i) == ')') {
            if (pcounter == 1) 
            {
                endofstring = i; // we now set the end of the string to the end p
                endreached = true; // set that we found the end of the string
            }
            if (i == (t.size() - 1)) {
                nosecondpart = true; // if we did actually hit the size 
                // of the string there is nothing afterward 
            }
            pcounter--;
        }
        // assuming we finish this we can set the i value
        // to the size of the string and therefore break out of it
        if (endreached == true)
        {
            i = t.size();
        }
    }
    
    // if there was no p's we can proceed normally
    if (pexist == false && pchecker == false) {
        return parse_string(t);
    }
    // continue on and deal with splitting the string
    // deal with the p's by splitting the beg - end based on p's
    string newtext = t.substr (begofstring, (endofstring - begofstring));

    // we can perform the task again 
    // if there was no second part we can 
    // end our function by returning the value of return check
    // what this essentially does is skip splitting
    // the string up in to the correct components 
    bool returncheck = checkbrackets(newtext);
    if (nosecondpart == true) {
        return returncheck;
    }
    
    bool enable = true;
    for (unsigned i = endofstring; i < t.size(); i++) {
        if (enable && t.at(i) == '&' && t.at(i + 1) == '&')
        {
            endofstring = i + 2;
            enable = false;
        }
        if (enable && t.at(i) == '|' && t.at(i + 1) == '|')
        {
            if (returncheck == true) {
                return true; 
            }
            endofstring = i + 2;
            enable = false;
        }
    }
    
    // take the substr of t again but this time
    // deals with everything after the end p
    string parsedagain = t.substr(endofstring, t.size() - endofstring);
    
    return checkbrackets(parsedagain);
}

int main()
{
    string t;               //accepting command line input 
    char* login=getlogin(); //getting the login
    char hostname[1024];    //hostname
    hostname[1023]='\0';    //CONVENTIONS FOR OBTAINING HOSTNAME
    int hostname_affirm=gethostname(hostname,30);
    while(1)
    {
        if(login==NULL || hostname_affirm!=0)   
        //checking if hostname is available
        cout<<"$ ";         
        //DISPLAYING THE DOLLAR PROMPT IF hostname 
        //is not available (like in cloud9)
        else
        cout<<login<<"@"<<hostname<<"$ ";   
        //display prompt if prompt is available
        getline(cin,t);
        if(t=="exit")
            exit(1);
        checkbrackets(t);
    }
    return 0;
}
