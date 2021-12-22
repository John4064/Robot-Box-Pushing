#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main() 
{
    const char * fname = "File.txt";
    
    // 1. create a new file, write some text and close the file.
    {
        ofstream fout(fname);
        fout << "This is a text file.\n";
    }
    
    // 2. open the file in append mode, write some more text and close.
    {
        ofstream fout(fname, ios::app);  
        fout << "Oranges and lemons.\n";      
    }
    
    // 3. open the file for input and display the contents.   
    ifstream fin(fname);
    
    string line;
    while (getline(fin, line))
        cout << line << '\n';
        
    return 0;
}

