#include <iostream>
#include <fstream>
#include<string>
#include <vector>
#include <sstream>
#include<windows.h>

using namespace std;

/**
 * detecting a specific continuous field in a line string */

void trim(string &s)
{
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
}

 bool detectContinuousField(const string & lineStr,const string & field)
{
    string::size_type idx;
    idx = lineStr.find(field);
    if(idx == string::npos)
        return false;
    else
        return true;
}

bool detectSpecificChar(const string & str,const char & c)
{
     string::size_type idx;
     idx = str.find(c);
     if(idx == string::npos)
         return false;
     else
         return true;
}

bool detectDiscontinuousField(const string & lineStr,const string & field)
{
     int strLength = field.length();
     vector<char> discontinuousField;
     for(int i = 0;i < strLength;i ++)
     {
         discontinuousField.push_back(field[i]);
     }

     for(char c : discontinuousField)
     {
         if(!detectSpecificChar(lineStr,c))
             return false;
     }
     return true;
}

bool detectChildrenString(const string & lineStr,vector<string> childrenString)
{
     for(string childStr : childrenString)
     {
         if(!detectContinuousField(lineStr,childStr))
             return false;
     }
     return true;
}

string switchNumberFormat(const int loopCount)
{
     stringstream ss;
     ss << loopCount;
     string targetStr;
     ss >> targetStr;
     int length = targetStr.length();

    switch (length) {
        case 1:
            targetStr = "000" + targetStr;
            break;
        case 2:
            targetStr = "00" + targetStr;
            break;
        case 3:
            targetStr = "0" + targetStr;
            break;
    }
    return targetStr;
}

vector<string> splitLineStr(const string & lineStr)
{
     vector<string> splitStrInLine;
     stringstream ss(lineStr);
     string str;
     while(getline(ss,str,','))
     {
         trim(str);
         splitStrInLine.push_back(str);
     }
     return splitStrInLine;
}




string getZeroOutput(const string lineStr)
{
     string zeroOutput = splitLineStr(lineStr)[2];
     trim(zeroOutput);
    return zeroOutput;
}


float similar(double h){

    long temp;

    /*
	h*1000?????????????????????????????????????????????+5?????????????????????????????????
	/10?????????????????????????????????????????????????????????????????????????????????
	??????????????????temp/100?????????????????????????????????????????????????????????????????????
	????????????????????????????????????
	*/
    temp = (h*1000 + 5)/10;

    return (float) temp/100;

}


string remainDecimal(string & str)
{
    string::size_type idx;
    idx = str.find('.');
    if(idx == string::npos)
        throw runtime_error("not a decimal error!");
    return str.substr(0,idx + 3);
}

string getSensitiveData(vector<vector<string>> sensitiveTable)
{
     double sum;
     for(int i = 0;i < 10;i ++)
     {
         string sensitiveData = sensitiveTable[i][4];
         sum = sum + atof(sensitiveData.c_str());
     }
     double average = sum / 10;
     float averageAfterSimilar = similar(average);
     string averageStr = to_string(averageAfterSimilar);
     return remainDecimal(averageStr);
}

string & deleteSpecificChar(string & str,char c)
{
    int pos = str.find(c);
    str.erase(pos,1);
    return str;
}

string getLinearity(string & str)
{
    vector<string> linearity = splitLineStr(str);
    string linearityData = linearity[1];
    string rightLinearityData = deleteSpecificChar(linearityData,'%');
    return rightLinearityData;
}

int main() {

     /**
      * generated file*/
      ofstream outFile;
      outFile.open("generated.csv",ios::out);
      if(!outFile)
      {
          cout << "file generated create or open failure!" << endl;
          throw runtime_error("file error!");
      }
      outFile << "??????" << "," << "????????????" << "," << "?????????" << "," << "?????????????????????" << "," << "?????????????????????"
            << "," << "??????????????????" << "," << "??????????????????" << "," << "???????????????" << "," << "??????????????????" << endl;
    /**
     * open target file*/
     ifstream inFile("data.csv",ios::in);
     if(!inFile)
     {
         SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
         cout << "Failure! file data.csv open failure.Maybe file not named data.csv or having no csv file in this directory!" << endl;
         system("pause");
         return 1;
     }
     string lineStr;

     /**
      * loop control*/

     int loopCount = 0;
     bool loopStart = false;
     bool recordSensitiveFinished = false;
     bool recordLinearityFinished = false;

     /**
      * some specific str needed to detect*/
     const vector<string> zeroDetectFlag = {"Limit Lo","Limit Hi"};
     const vector<string> sensitiveDetectFlag = {"Sensitive","Vzero"};
     const vector<string> linearityDetectFlag = {"Vcalc","Delta"};
     const vector<string> accuracyDetectFlag = {"V1","V2"};


     while(getline(inFile,lineStr))
     {
         /**
          * zero output */


         if(detectChildrenString(lineStr,zeroDetectFlag) && !loopStart &&
                 !detectChildrenString(lineStr,sensitiveDetectFlag))
         {
             loopStart = true;
             loopCount ++;
             outFile << switchNumberFormat(loopCount) << ",";
             /**
              * record zero output*/
             getline(inFile,lineStr);
             outFile << getZeroOutput(lineStr) << ",";
             cout << "loop " << loopCount << ": zeroOutput finished" << endl;
             continue;
         }

         /**
          * record sensitive*/
          if(detectChildrenString(lineStr,sensitiveDetectFlag) && loopStart)
          {
              /**
               * get ten data*/
              vector<vector<string>> sensitiveTable;
              for(int i = 0;i < 10; i ++)
              {
                  getline(inFile,lineStr);
                  vector<string> sensitiveLine = splitLineStr(lineStr);
                  sensitiveTable.push_back(sensitiveLine);
              }
              string sensitiveAverage = getSensitiveData(sensitiveTable);
              outFile << sensitiveAverage << ",";
              recordSensitiveFinished = true;
              cout << "loop " << loopCount << ": sensitive detect finished" << endl;
              continue;
          }

          if(detectChildrenString(lineStr,linearityDetectFlag) && recordSensitiveFinished)
          {
              for(int i = 0;i < 13;i ++)
              {
                  getline(inFile,lineStr);
              }
              if(!detectContinuousField(lineStr,"Vout"))
                  throw runtime_error("detect continuous word ????????? error");
              getline(inFile,lineStr);

              outFile << getLinearity(lineStr) << ",";
              cout << "loop " << loopCount << ": linearity positive detect finished" << endl;

              for(int i = 0;i < 13;i ++)
                  getline(inFile,lineStr);
              if(!detectContinuousField(lineStr,"Vout"))
                  throw runtime_error("detect continuous word ?????????(negative) error");
              getline(inFile,lineStr);
              outFile << getLinearity(lineStr) << ",";
              recordLinearityFinished = true;
              cout << "loop " << loopCount << ": linearity negative detect finished" << endl;
              continue;

          }

          if(detectChildrenString(lineStr,accuracyDetectFlag) && recordLinearityFinished)
          {
              for(int i = 0; i < 23;i ++)
                  getline(inFile,lineStr);
              if(!detectContinuousField(lineStr,"Vout"))
                  throw runtime_error("detect continuous word ?????? error");

              getline(inFile,lineStr);
              outFile << getLinearity(lineStr) << endl;
              loopStart = false;
              recordSensitiveFinished = false;
              recordLinearityFinished = false;
              cout << "loop " << loopCount << ": accuracy detect finished" << endl;
              continue;
          }
     }

     if(loopCount != 0)
     {
         SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
         cout << "Successfully! Successfully Created generated.csv File,Please Close Terminal And View it!" << endl;
     }
     else
     {
         SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
         cout << "Failure! data.csv doesn't be encoded in UTF-8 or the file doesn't have content!" << endl;
     }

     system("pause");
      return 0;
}
