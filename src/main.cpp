#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
using namespace std;
namespace fs = std::filesystem;

int main() {
  // Flush after every std::cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  // TODO: Uncomment the code below to pass the first stage

  while(true){
    cout << "$ ";
    string input;
    getline(cin, input);

    if(input == "exit") {
      break;
    }

    else if (input.substr(0, 5) == "echo "){

      if (input.length() < 4) continue;
      string args = input.substr(5);
      if (args.find('\'') != string::npos ){
        for (int i = 0; i < args.length(); i++){
          if (args[i] == '\''){
            continue;
          }else{
            cout << args[i];
          }
        }
        cout<<endl;

      }else{
        stringstream ss(args);
        vector<string> tokens;
        string word;

        while(ss >> word){
          tokens.push_back(word);
        }

        for(size_t i =0; i< tokens.size(); i++){
          cout<<tokens[i];
          if(i < tokens.size()-1){
            cout << " ";
          }
        }
        cout<<endl;
      }

    }

    else if ( input.substr(0, 5)== "type "){

        if (input.substr(5) == "type" || input.substr(5) == "exit" || input.substr(5) == "echo" || input.substr(5) == "pwd") {
          cout<< input.substr(5) << " is a shell builtin "<<endl;
        }else {

          char *path_env = getenv("PATH");

          if (path_env != nullptr && *path_env != '\0') {
            string full_path = path_env;
            replace(full_path.begin(), full_path.end(), ';', ':');
            stringstream ss(full_path);
            string folder_path;
            bool found = false;

            while (getline(ss, folder_path, ':')) {

              fs::path folder_path1 = folder_path + "/" + input.substr(5);


              if (fs::exists(folder_path1) && (fs::status(folder_path1).permissions() & fs::perms::owner_exec) != fs::perms::none) {
                found = true;
                cout << input.substr(5) << " is " << folder_path + "/" + input.substr(5) << endl;
                break;
              }
            }
            if (!found) {
                cout << input.substr(5) << ": not found" << endl;
            }
          }else {
            cout<< "PATH environment variable is not set." << endl;
          }

        }

    }

    else if(input == "pwd" || input.substr(0, 4) == "pwd "){
      // cout << fs::current_path().string()<< endl; // ممكن تستخدم دي
      char* dynamic_cwd = getcwd(nullptr, 0);
      if(dynamic_cwd != nullptr){
        cout<<dynamic_cwd<<endl;
        free(dynamic_cwd);
      }else{
        perror("Dynamic getcwd faild");
      }
    }

    else if (input.substr(0,2) == "cd"){
      string path;
      if (input == "cd" || input == "cd " || input.substr(3) == "~"){
          char* home = getenv("HOME");
          path = (home != nullptr)? home : "/" ;

      }else{
        path = input.substr(3);
      }

      if (chdir(path.c_str()) == 0) {
      } else {
        cout << "cd: " << path << ": No such file or directory" << endl;
      }
    }


    else{
      stringstream sc;

      vector <string> command;
      string token;
      sc << input;

      while (sc >> token){
        command.push_back(token);
      }

      char *c_words[command.size() + 1];

      for(size_t i = 0;  i < command.size(); i++ ){
        c_words[i] = const_cast<char*>(command[i].c_str());
      }

      c_words[command.size() ] = nullptr;

      pid_t pid = fork();
      if(pid < 0){
        perror("Fork failed");
        return 1;
      }
      else if(pid == 0){
          execvp(c_words[0], c_words);
          cout << c_words[0] << ": command not found" << endl;
          exit(1);
      }
      else{
        int status;
        waitpid(pid, &status, 0);
      }
    }

  }

}
