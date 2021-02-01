/*
Universidad del Valle de Guatemala
Programacion de microporcesadores
10/09/2020
Decrypting program
*/


/* Including all libraries */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace std;

/* Declarating variables */
pthread_t tid[4]; //Threads
int counter; // Threads counter
char res1, res2, res3, res4; // Threads results

/*Function to convert a number to base 10
Code from: https://gist.github.com/JohannesMP/a911b7dc02bb0586e3111a0cbd2dc0e2*/
int convertTo10(const string& input, int base){
    if(base < 2 || base > 36)
        return 0;

    bool isNegative = (input[0] == '-');

    int startIndex = input.length()-1;
    int endIndex   = isNegative ? 1 : 0;

    int val = 0;
    int digitVal = 1;

    for(int i = startIndex; i >= endIndex; --i)
    {
        char c = input[i];

        if(c >= 'a' && c <= 'z')
            c -= ('a' - 'A');

        if(c >= '0' && c <= '9')
            c -= '0';
        else
            c = c - 'A' + 10;

        if(c >= base)
            return 0;

        val += c * digitVal;

        digitVal *= base;
    }

    if(isNegative)
        val *= -1;

    return val;
}

/* Function to convert to binary number string */
string toBin(int n){
    string r;
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
    while(r.size()<8){r = "0" + r;}
    return r;
}

/* Function to obtain de shift using the key*/
int getShift(string key){
    int shift = convertTo10(key, 36)%10;
    return shift;
}

/* Function that operates an xor to the binary chains*/
string xorStr(string str1, string str2){
    string xorstr = "";
    for(int x=0; x<str1.size();x++){
        xorstr += (str1[x]!=str2[x]?"1":"0");
    }
    return xorstr;
}

struct thread_data {
    char key; //Encrypting key
	int id;  // Thread ID
	int shift; //shift for the encrypting offset
	char data; //Data vector
};



/* Function to save all data from results */
void fillData(int index, char data){
	index = index%4;
	if(index == 0){
		res1 = data;
	} else if(index == 1){
		res2 = data;
	} else if(index == 2){
		res3 = data;
	} else if(index == 3){
		res4 = data;
	}
}


/* Encrypting function */


void strToBinary(string s) 
{ 
    int n = s.length(); 
  
  
    for (int i = 0; i <= n; i++) 
    { 
        // convert each char to 
        // ASCII value 
        int val = int(s[i]); 
  
        // Convert ASCII value to binary 
        string bin = ""; 
        while (val > 0) 
        { 
            (val % 2)? bin.push_back('1') : 
                       bin.push_back('0'); 
            val /= 2; 
        } 
        reverse(bin.begin(), bin.end()); 
  
        cout << bin << " "; 
    } 
} 

void *decrypt(void *threadarg){
	struct thread_data *my_data = (struct thread_data *)threadarg;
	char key = my_data->key;
	char text = my_data->data;
	int thread_UID = my_data->id;
	int shift = my_data->shift;
    //Converting the key to binary
    string binKey = toBin(int(key));
    //Offset shift ascii mod 256
    string temp = toBin(int(text));
    //xor char from key to char from string
    temp = xorStr(temp, binKey);
    // Xored bin string back to ascii in dec
    text=(convertTo10(temp, 2)-shift)%256;
    fillData(thread_UID, text);
	
	
	}
	


int main()
{
    //pthread
    int err;
    // Key and shift
    string key = "jjdj";
    int keyShift = getShift(key);
    /* Open the file */
    ifstream inputFile;
    inputFile.open("encryptedText.txt");
    ofstream outputFile;
    outputFile.open("decryptedText.txt");
    /* ------------ */

    // Cicle counter
    int cont = 0;
    // Chain counter
    int cDt = 0;

    char caracteres[4];
    struct thread_data dt[4];
    while(!inputFile.eof()){
        cont = 0;

        for(int c1 = 0; c1<4;c1++){
            inputFile.get(caracteres[c1]);
            dt[c1].key = key[cDt];
            dt[c1].data = caracteres[c1];
            dt[c1].shift = keyShift;
            dt[c1].id = c1;
            cDt =(1+cDt)%key.size();
        }

        //Enceypting characters 4 by 4
        while(cont<4){
            err =pthread_create(&tid[cont], NULL, decrypt, (void *)&dt[cont]);
            if (err != 0){printf("\ncan't create thread :[%s]", strerror(err));}
            cont++;
        }

        pthread_join(tid[0], NULL);
        pthread_join(tid[1], NULL);
        pthread_join(tid[2], NULL);
        pthread_join(tid[3], NULL);

        //Write the results
        outputFile << res1;
        outputFile << res2;
        outputFile << res3;
        outputFile << res4;

    }
    inputFile.close();
    outputFile.close();
    pthread_exit(NULL);
    
    
    return 0;
}

