#include <cstdlib>
#include <iostream>
#include <string>
#include <string.h>
#include <cstring>
#include <sstream>
#include<vector>
#include<algorithm>
#include <unistd.h>
#include <stdio.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include<stdint.h>
#include<ctype.h>

//typedef unsigned long int uint32_t;
//typedef unsigned long long int uint64_t; 
typedef std::vector<std::string> StrVec;
typedef std::vector<StrVec> StrVecVec;

void parse_and_run_command(const std::string &command) {
    /* TODO: Implement this. */
    /* Note that this is not the correct way to test for the exit command.
       For example the command "   exit  " should also exit your shell.
     */
    if (command == "exit") {
        exit(0);
    }
    
    //1. Eliminate Token similar to whiteSpcae
    std::string commandLine = command;
    std::string cmdLine;
    bool lastTokenValid=false;
    for(uint32_t i=0;i<commandLine.size();i++)
    {
        std::string t;
        std::stringstream stream1;
        stream1 << commandLine[i];
        t = stream1.str();
        if(lastTokenValid==true)
        {
            if(commandLine[i]==' '|| isblank(commandLine[i]))
            {
                t=" ";
                lastTokenValid=false;
                
            }
            else if(commandLine[i]=='\\' && (i+1)<commandLine.size())
            {
                if( (commandLine[i+1]=='t')||(commandLine[i+1]=='f')||(command[i+1]=='n')||(commandLine[i+1]=='r')||(commandLine[i+1]=='v'))
                {
                    t=" ";
                    i=i+1;
                    lastTokenValid=false;
                }
            } 
            cmdLine.append(t);
            continue;
        }
        else
        {
            if(commandLine[i]==' '|| isblank(commandLine[i]))
            {
                continue;
            }
            else if(commandLine[i]=='\\' && (i+1)<commandLine.size())
            {
                if( (commandLine[i+1]=='t')||(commandLine[i+1]=='f')||(command[i+1]=='n')||(commandLine[i+1]=='r')||(commandLine[i+1]=='v'))
                {
                    i=i+1;
                    continue;
                }
                else
                {
                    lastTokenValid=true;
                    cmdLine.append(t);
                }
            } 
            else
            {
                lastTokenValid=true;
                cmdLine.append(t);
            }
        }
    }
    if(cmdLine.empty()==false)
    {
        if(cmdLine.at(0)==' '){
            std::cout<<"{AAAAAAA"<<std::endl;
        }
        char str_end=cmdLine.at(cmdLine.length()-1);
        if(str_end==' ')
        {
            cmdLine.erase(cmdLine.end()-1);
        }
    }
    //Detect Invalid
    if(cmdLine.empty()==false)
    {
        char str_end=cmdLine.at(cmdLine.length()-1);
        if(cmdLine.length()>1)
        {
            char str_end2=cmdLine.at(cmdLine.length()-2);
            if((str_end=='|' || str_end=='>' || str_end=='<')&&(str_end2==' '))
            {
                std::cerr<<"invalid command."<<std::endl;
                return;
            }
        }
        else
        {
            if(str_end=='|' || str_end=='>' || str_end=='<')
            {
                std::cerr<<"invalid command."<<std::endl;
                return;
            }
        }
    }
    //std::cout<<"cmdLine-{"<<cmdLine<<"}-"<<std::endl;

    StrVec Cmds;Cmds.clear();
    std::string str_cmds[100];
    uint32_t cmd_args_index=0;
    while(cmdLine.empty()==false)
    {
        int pipe_pos=-1;
        for(uint32_t i=0;i<(cmdLine.size()-2);i++)
        {
            if((cmdLine[i]==' ')&&(cmdLine[i+1]=='|')&&(cmdLine[i+2]==' '))
            {
                pipe_pos=(i+1);
                break;
            }
        }

        if(pipe_pos!=-1)
        {
            uint32_t cmd_end_pos = pipe_pos;
            for(uint32_t j=0;j<cmd_end_pos;j++)
            {
                str_cmds[cmd_args_index]+=cmdLine.at(0);
                cmdLine.erase(cmdLine.begin());
            }
            cmdLine.erase(cmdLine.begin());
            cmd_args_index++;
            continue;
        }
        else
        {
            str_cmds[cmd_args_index]=cmdLine;
            cmd_args_index++;
            break;
        }
    }
    for(uint32_t i=0;i<cmd_args_index;i++)
    {
        while (str_cmds[i].empty()==false && str_cmds[i].at(0)==' ')
        {
            str_cmds[i].erase(str_cmds[i].begin());
        }
        while (str_cmds[i].empty()==false && str_cmds[i].at(str_cmds[i].length()-1)==' ')
        {
            str_cmds[i].erase(str_cmds[i].end()-1);
        }
        Cmds.push_back(str_cmds[i]);
    }
    //Detect Invalid Command
    StrVecVec cmd_args_vec;
    for(StrVec::iterator cmd_itr=Cmds.begin();cmd_itr!=Cmds.end();cmd_itr++)
    {
        std::string single_cmd = (*cmd_itr);
        //std::cout<<"CMD-{"<<single_cmd<<"}-"<<std::endl;
        StrVec args;
        std::string tmp_arg;
        while(single_cmd.empty()==false)
        {
            char headChar=single_cmd.at(0);
            if(headChar!=' ')
            {
                tmp_arg+=headChar;
            }
            else
            {   //std::cout<<"tmp_arg-{"<<tmp_arg<<"}-"<<std::endl;
                if(tmp_arg.empty()==false)
                {
                    args.push_back(tmp_arg);
                }
                tmp_arg.clear();
            }
            single_cmd.erase(single_cmd.begin());
        }
        args.push_back(tmp_arg);
        cmd_args_vec.push_back(args);
    }

    /*
    std::cout<<"PNUM-"<<cmd_args_vec.size()<<std::endl;
    for(StrVecVec::iterator i=cmd_args_vec.begin();i<cmd_args_vec.end();i++)
    {
        std::cout<<"CMD"<<i->size()<<"-{"<<std::endl;
        for(StrVec::iterator j=(*i).begin();j<(*i).end();j++)
        {
            std::cout<<"ARG-["<<*j<<"]-"<<std::endl;
        }
    }*/

    std::vector<int> pid_Vec;
    std::vector<int>::iterator p_itr;
    int fd[2];
    int init_input=dup(0);
    for(StrVecVec::iterator i=cmd_args_vec.begin();i!=cmd_args_vec.end();i++)
    {
        bool pipe_end=((i+1)==cmd_args_vec.end() ? true:false);
        
        if(pipe(fd)<0)
        {
            perror(nullptr);
            return;
        }

        pid_t pid;
        if((pid=fork())<0)
        {
            perror(nullptr);
            return;
        }

        if(pid == 0)//Child
        {
            if(pipe_end==false)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]); 
            }
            StrVec targs = (*i);
            std::string inFileName, outFileName;
            //Detect Invalid Command
            int rdirTokenNum=0;
            for(StrVec::iterator it=targs.begin();it!=targs.end();it++)
            {
                std::string tmpStr=(*it);
                if(tmpStr==">")
                {
                    std::string rdirStr=*(it+1);
                    outFileName=rdirStr;
                    rdirTokenNum++;
                }
                if(tmpStr=="<")
                {
                    std::string rdirStr=*(it+1);
                    inFileName=rdirStr;
                    rdirTokenNum++;
                }
            }
            if(rdirTokenNum>1)
            {
                std::cerr<<"invalid command."<<std::endl;
                return;
            }

            //Redirection
            if(inFileName.empty()==false)
            {
                int fd_in=open(inFileName.c_str(),O_RDONLY);
                if(fd_in == -1){
                    perror(nullptr);
                    return;
                }
                close(STDIN_FILENO);
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if(outFileName.empty()==false)
            {
                int fd_out=open(outFileName.c_str(),O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
                if(fd_out == -1){
                    perror(nullptr);
                    return;
                }
                close(STDOUT_FILENO);
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }

            char *args[500]={nullptr};
            int arg_index=0;
            for(StrVec::iterator c_args_itr=targs.begin();c_args_itr!=targs.end();c_args_itr++)
            {
                std::string tmp_str=(*c_args_itr);
                args[arg_index] = new char[500];
                strcpy(args[arg_index],tmp_str.c_str());
                arg_index++;
            }
            if(execv(args[0],args)==-1)
            {
                perror(nullptr);
                return;
            }    
        }
        else//Parent
        {
            if(pipe_end==false)
            {
                close(STDIN_FILENO);
                dup2(fd[0],STDIN_FILENO);
                close(fd[1]);
                close(fd[0]);
            }
            pid_Vec.push_back(pid);
        }
    }
    int p_index=0;
    for(p_itr=pid_Vec.begin();p_itr!=pid_Vec.end();p_itr++)
    {
        int stat;
        if(waitpid(*p_itr,&stat,0)==-1)
        {
            perror(nullptr);
            return;
        }

        int e_st=0;
        bool err=false;
        if (WIFEXITED(stat)) {e_st = WEXITSTATUS(stat);}
        else if (WIFSIGNALED(stat)){err = true;}
        
        StrVecVec::iterator ca_itr = cmd_args_vec.begin();
        StrVec arglist = *(ca_itr+p_index);
        for(StrVec::iterator arg_iter =arglist.begin(); arg_iter != arglist.end(); arg_iter++)
        {
            std::cout << *arg_iter << " ";
        }
        if (err == false)
        {
            std::cout << "exit status: "<<e_st<<std::endl;
        }
        p_index++;
    }
    dup2(init_input, STDIN_FILENO);
    close(init_input);
}

int main(void) {
    std::string command;
    std::cout << "> ";
    while (std::getline(std::cin, command)) {
        parse_and_run_command(command);
        std::cout << "> ";
    }
    return 0;
}
