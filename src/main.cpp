#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
namespace fs = std::filesystem;

struct Command
{
  string name;
  string output_file;
  vector<string> args;
  bool redirect_stdout;
};

void parse_redirection(Command& cmd){
  cmd.redirect_stdout = false;
  cmd.output_file = "";

  for ( size_t i = 0; i < cmd.args.size(); ++i ){
    if ( cmd.args[i]  == ">" || cmd.args[i] == "1>" ){

      if (i + 1 < cmd.args.size()){
        cmd.output_file = cmd.args[i+1];
        cmd.redirect_stdout = true;
        cmd.args.erase(cmd.args.begin() + i );
        cmd.args.erase(cmd.args.begin() + i );
      }
      break;
    }
  }
}

Command parse_input(const string& input ) {
  bool in_double_quotes = false;
  bool in_single_quotes = false;
  bool back_slash = false;

  vector<string> tokens;
  Command cmd;
  string conten = "";

  for (int i = 0; i < input.length(); i++){
    char current = input[i];

    if (current == '\\' ){
      if (back_slash || in_single_quotes){
        conten += current;
        back_slash = false;
      }else{
        back_slash = true;
      }
      continue;
    }

    if (current == '\"' ) {
      if (in_single_quotes || back_slash){
        conten += current;

      }else{
        in_double_quotes =! in_double_quotes;
      }
      back_slash = false;
      continue;
    }

    if (current == '\'' ){
      if (in_double_quotes || back_slash){
        conten += current;

      }else{
        in_single_quotes =! in_single_quotes;
      }
      back_slash = false;
      continue;
    }


    if (current == ' '){
      if (!in_single_quotes && !in_double_quotes && !back_slash){
        if (!conten.empty()){
          tokens.push_back(conten);
          conten = "";
        }
        back_slash = false;
        continue;
      }
    }


    back_slash = false;
    conten += current;

  }

  if (!conten.empty()){
    tokens.push_back(conten);
  }

    if (!tokens.empty()){
      cmd.name = tokens[0];

      for (size_t i = 1; i < tokens.size(); i++){
        cmd.args.push_back(tokens[i]);
      }
    }

  parse_redirection(cmd);
  return cmd;


}



void handle_echo(const Command& cmd){

  for (size_t i = 0; i < cmd.args.size(); i++) {
    cout << cmd.args[i];
    if (i < cmd.args.size() - 1) {
      cout << " ";
    }
  }
  cout << endl;
}

void handle_type(const Command& cmd){

  if (cmd.args[0] == "type" || cmd.args[0] == "exit" || cmd.args[0] == "echo" || cmd.args[0] == "pwd") {
    cout<< cmd.args[0] << " is a shell builtin "<<endl;
  }else {

    char *path_env = getenv("PATH");

    if (path_env != nullptr && *path_env != '\0') {
      string full_path = path_env;
      replace(full_path.begin(), full_path.end(), ';', ':');
      stringstream ss(full_path);
      string folder_path;
      bool found = false;

      while (getline(ss, folder_path, ':')) {

        fs::path folder_path1 = folder_path + "/" + cmd.args[0];


        if (fs::exists(folder_path1) && (fs::status(folder_path1).permissions() & fs::perms::owner_exec) != fs::perms::none) {
          found = true;
          cout << cmd.args[0] << " is " << folder_path + "/" + cmd.args[0] << endl;
          break;
        }
      }
      if (!found) {
        cout << cmd.args[0] << ": not found" << endl;
      }
    }else {
      cout<< "PATH environment variable is not set." << endl;
    }

  }

}

void handle_pwd( const Command& cmd ){
  // cout << fs::current_path().string()<< endl; // ممكن تستخدم دي
  char* dynamic_cwd = getcwd(nullptr, 0);
  if(dynamic_cwd != nullptr){
    cout<<dynamic_cwd<<endl;
    free(dynamic_cwd);
  }else{
    perror("Dynamic getcwd faild");
  }
}

void handle_cd( const Command& cmd ){

  string path;
  if ( cmd.args.empty() || cmd.args[0] == "~" ){
    char* home = getenv("HOME");
    path = (home != nullptr)? home : "/" ;

  }else{
    path = cmd.args[0];
  }

  if (chdir(path.c_str()) == 0) {
  } else {
    cout << "cd: " << path << ": No such file or directory" << endl;
  }

}

void execute_external( const Command& cmd ){

  char *argv[cmd.args.size() + 2];
  argv[0] = const_cast<char*>(cmd.name.c_str());

  for(size_t i = 1;  i <= cmd.args.size(); i++ ){
    argv[i] = const_cast<char*>(cmd.args[i-1].c_str());
  }

  argv[cmd.args.size() + 1] = nullptr;

  pid_t pid = fork();
  if(pid < 0){
    perror("Fork failed");
    exit(-1);
  }
   if(pid == 0)
   {
     if (cmd.redirect_stdout){
       int fd = open( cmd.output_file.c_str() , O_WRONLY | O_CREAT | O_TRUNC, 0644);
       if (fd == -1) {
         perror("Error creating file");
         exit(1);
       }
       dup2(fd, STDOUT_FILENO);
       close(fd);
     }
       execvp(argv[0], argv);
       cout << argv[0] << ": command not found" << endl;
       exit(1);
   }else{
    int status;
    waitpid(pid, &status, 0);
  }
}

void execute_line(const string& input)
{
    Command cmd = parse_input(input);
    if(cmd.name == "exit") {
        exit(0);
      }

    else if (cmd.name == "echo"){
      handle_echo(cmd);
    }

    else if ( cmd.name== "type"){
       handle_type( cmd);
    }

    else if(cmd.name == "pwd"){
       handle_pwd(cmd);
    }

    else if (cmd.name == "cd"){
       handle_cd( cmd);
    }

    else{
      execute_external(cmd);
    }

}


int main() {
  // Flush after every std::cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  // TODO: Uncomment the code below to pass the first stage

  while(true){
    cout << "$ ";
    string input;
    getline(cin, input);
    execute_line(input);


  }

}


