#include <iostream>
#include <fstream>
#include<string>
#include <vector>
#include <sstream>

using namespace std;

/**
 * detecting a specific continuous field in a line string */
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
         splitStrInLine.push_back(str);
     }
     return splitStrInLine;
}

string getZeroOutput(const string lineStr)
{
    return splitLineStr(lineStr)[2];
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
     ifstream inFile("data.vsv",ios::in);
     string lineStr;
     int loopCount = 0;
     bool loopStart = false;
     bool recordSensitiveFinished = false;
     while(getline(inFile,lineStr))
     {
         /**
          * zero output */
         if(detectDiscontinuousField(lineStr,"零点") && !loopStart)
         {
             loopStart = true;
             loopCount ++;
             outFile << switchNumberFormat(loopCount) << ",";
             /**
              * record zero output*/
             getline(inFile,lineStr);
             getline(inFile,lineStr);
             outFile << getZeroOutput(lineStr) << ",";
             continue;
         }

         /**
          * record sensitive*/
          if(detectDiscontinuousField(lineStr,"灵敏度") && loopStart)
          {
              getline(inFile,lineStr);
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

          if(detectDiscontinuousField(lineStr,"线性度") && loopStart && recordSensitiveFinished)
          {
              for(int i = 0;i < 15;i ++)
              {
                  getline(inFile,lineStr);
              }
              if(!detectContinuousField(lineStr,"线性度"))
                  throw runtime_error("detect continuous word 线性度 error");
              vector<string> linearity = splitLineStr(lineStr);
              string linearityData = linearity[1];
              linearityData.erase(std::remove(linearityData.begin(),linearityData.end(),'%'),linearityData.end());
          }
     }

      return 0;
}
