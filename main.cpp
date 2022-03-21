#include <iostream>
#include <fstream>
#include<string>
#include <vector>
#include <sstream>

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

string getSensitiveData(vector<vector<string>> sensitiveTable)
{
     double sum;
     for(int i = 0;i < 10;i ++)
     {
         string sensitiveData = sensitiveTable[i][4];
         sum = sum + atof(sensitiveData.c_str());
     }
     double average = sum / 10;
     return to_string(average);
}

int main() {

     /**
      * generated file*/
      ofstream outFile;
      outFile.open("generated.csv",ios::out);
      outFile << "编号" << "," << "零点输出" << "," << "灵敏度" << "," << "线性度（正向）" << "," << "线性度（负向）"
            << "," << "精度（正向）" << "," << "精度（负向）" << "," << "热零点漂移" << "," << "热灵敏度漂移" << endl;
    /**
     * open target file*/
     ifstream inFile("data4.csv",ios::in);
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
              continue;
          }

          if(detectChildrenString(lineStr,linearityDetectFlag) && recordSensitiveFinished)
          {
              for(int i = 0;i < 13;i ++)
              {
                  getline(inFile,lineStr);
              }
              if(!detectContinuousField(lineStr,"Vout"))
                  throw runtime_error("detect continuous word 线性度 error");
              getline(inFile,lineStr);
              vector<string> linearity = splitLineStr(lineStr);

              string linearityData = linearity[1];

          }
     }

      return 0;
}
