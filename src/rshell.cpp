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
    unsigned commandlinestartup_length = 
    commandlinestartup.length() + 1; 
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
    char* argv[3];  
    //the argv pointer array goes into the execvp function
    //, which is described inthe main
    if(argumentative.length() == 0)                
    //pages as follows: execvp(char* argv[0], argv )
    {
        argv[0] = command;       
        // single word commands do not 
        // have arguments so the others are null
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
        memcpy(commandtorun, singlecommand.c_str(), 
        singlecommand.size() + 1);
        int m;
        
        return execute(commandtorun, m);
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
        t = t.substr(0, t.find("#")); 
        // shorten down the command line by 
    //removing the #
    }
    nocomments = t;
    nocomments.erase(remove(nocomments.begin(), nocomments.end(), '"'), 
    nocomments.end()); // removes all quotation marks from nocomments
    string temp;
    string newtemp;
    string semitemp;
    // Tests against multiple semicolons
    // Bug: However does not work if there is a space between them
    // As it breaks the code
    for (unsigned int i = 0; i < nocomments.size() - 1; i++) {
        if (nocomments.at(i) == ';' && nocomments.at(i + 1) == ';') {
            cout << "Syntax Error!" << endl;
            return true;
        }
    }
    for (unsigned int i = 0; i < nocomments.size() - 1; i++) {
        if (nocomments.at(i) == '&' && nocomments.at(i + 1) == '&') {
            cout << "Syntax Error!" << endl;
            return true;
        }
    }
    for (unsigned int i = 0; i < nocomments.size() - 1; i++) {
        if (nocomments.at(i) == '|' && nocomments.at(i + 1) == '|') {
            cout << "Syntax Error!" << endl;
            return true;
        }
    }
    // This code deals with anything after && that causes syntax error
    for (unsigned int i = 0; i < nocomments.size(); i++) { 
        // used to find any extra repeating semi colons
        if (nocomments.at(i) == '&' && nocomments.at(i+1) == '&') {
            temp = nocomments.substr(i + 2);
            break;
        }
    }
    for (unsigned int i = 0; i < temp.size(); i++) {
        if (temp.at(i) == '&' || temp.at(i) == '|') {
            cout << "Syntax Error!" << endl;
            return true;
        }
    }
    
    //This test case deals with anything 
    //after || that causes syntax error
    for (unsigned int i = 0; i < nocomments.size(); i++) { 
        // used to find any extra repeating semi colons
        if (nocomments.at(i) == '|' && nocomments.at(i+1) == '|') {
            newtemp = nocomments.substr(i + 2);
            break;
        }
    }
    for (unsigned int i = 0; i < newtemp.size(); i++) {
        if (newtemp.at(i) == '|' || newtemp.at(i) == '&') {
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
    
    vector <string> cmd; // dealing with semi colons
    char* cstring = new char[nocomments.size() + 1];
    strcpy(cstring,nocomments.c_str());
    vector <string> subcmd; 
    // CASE 4 used to handle connectors inside semi colon command
    vector <string> subsubcmd; 
    // used to handle normal commands (no connectors)
    if (semi && aand && oor) { //handles the test 
		// case of a single command
        string singlecommand; 
        char_separator<char> removespaces(" "); // takes out the " " 
        tokenizer<char_separator<char> > token_s(nocomments, 
        removespaces); 
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
                // makes sure the first passed 
                //in string does not have a space
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
        memcpy(commandtorun, singlecommand.c_str(), 
        singlecommand.size() + 1); 
        // helps with memory leak
        int m;
        
        return execute(commandtorun, m);
        
    }
        // CASE 2
        // Dealing with multiple commands chained together with semi
        // SINCE NOT USING BOOST WE MUST EXECUTE IT DIFFERENTLY

    else if (!semi && aand && oor) {
        
        char *token = strtok(cstring, ";"); 
        // Had difficulty manipulating semicolons
        // using BOOST decided to make 
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
       
        vector<char> connector; 
        // identify where all the && or || are present 

        for(unsigned i=0;i<nocomments.length()-1;)        
        {
            if(nocomments.at(i)=='&' && nocomments.at(i+1)=='&')  
            // this if looks at if there are two &&s together
            {
                if(nocomments.at(i+2)=='&' || 
                nocomments.at(i+2)=='|') {
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
                if(nocomments.at(i+2)=='&' 
                || nocomments.at(i+2)=='|') {
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
        
        connector.push_back('\0'); 
        // pushback to signal endof the vector
        bool aandandoorchecker = true;
        bool aflag=true;                                    
                                                           
        char_separator<char> sepnotamp("&|");                              
        tokenizer<char_separator<char> > 
        token_notamp(nocomments,sepnotamp);  
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
    // Dealing with both commands chained
    // together with semis and connectors 
    // implementing the same idea as just semicolons
    //first by removing all semicolons then dealing with
    // connectors

    else if( (!aand || !oor) && !semi )                   
    {                     
        string temp;
        for (unsigned int i = 0; i < nocomments.size(); i++) { 
            // used to find any extra repeating semi colons
            if (nocomments.at(i) == ';') {
                temp = nocomments.substr(i + 1, 
                nocomments.length() - 1);
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
        // Had difficulty manipulating semicolons
        // using BOOST decided to make 
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
        } 
        // devise a way to separate 
        // the next condition IF it has && or ||
        //figure out a way to chronologically 
        // evaluate both subcmd or subsubcmd
        // by push backing the keyword FALSE we can 
        //differentiate what order the code should be in
        // since only either subsubcmd 
        // or subcmd can be FALSE at one time 
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
                    if(figureitout.at(z)=='&' 
                    && figureitout.at(z+1)=='&')  
                    // If we can identify && and ||s 
                    // run against possible test case
                    // then do similar to Case 2 
                    // and push back into connector
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
                    if(figureitout.at(z)=='|' 
                    && figureitout.at(z+1)=='|')  
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
                
                connector.push_back('\0'); 
                // signals the end of connector
                bool aandandoorchecker = true;
                bool aflag=true;                                    
                                                                   
                char_separator<char> sepnotamp("&|");                              
                tokenizer<char_separator<char> > token_notamp(
                subcmd.at(i),
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
                    { // if aandandoorchecker is 
					//true and the conncetor is &
                    // run simple_parse_string
                        aandandoorchecker=simple_parse_string(t);
                        b++;
                    }
                    else if(connector.at(b)=='|' && 
                    aandandoorchecker==false)  
                    {  //similar to above but 
			// aandanoorchecker MUST be false
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
return false;
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
        parse_string(t);
    }
    return 0;
}
