#include <iostream>
#include <string>
using namespace std;

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
    } else if (input.substr(0, 5) == "echo "){
      cout<<input.substr(5)<<endl;
    } else if ( input.substr(0, 5)== "type "){

      if (input.substr(5) == "type" || input.substr(5) == "exit" || input.substr(5) == "echo" ) {
      cout<< input.substr(5) << " is a shell builtin "<<endl;
      }else {
      cout << input.substr(5) << ": not found" << endl;
      }
    }

    else{
      cout << input<< ": command not found" << endl;
    }
  }
}
