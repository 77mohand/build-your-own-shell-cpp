#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <sstream>
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
      cout<<input.substr(5)<<endl;
    }

    else if ( input.substr(0, 5)== "type "){

        if (input.substr(5) == "type" || input.substr(5) == "exit" || input.substr(5) == "echo" ) {
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

    else{
      cout << input<< ": command not found" << endl;
    }

  }
}
