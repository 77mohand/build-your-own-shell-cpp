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
    }
    if (input.substr(0, 5) == "echo "){
      cout<<input<<endl;
    }
    else
    {
      cout << input << ": command not found" << endl;
    }
  }
}
