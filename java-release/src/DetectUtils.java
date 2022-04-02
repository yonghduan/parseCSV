public class DetectUtils {

    public static boolean detectContinuousField(String lineStr,String field)
    {
        return lineStr.contains(field);
    }

    public static boolean detectChildrenString(String lineStr,String[] childrenString)
    {
        for(String childStr : childrenString)
        {
            if(!detectContinuousField(lineStr,childStr))
                return false;
        }
        return true;
    }

    public static String[] splitLineStr(String lineStr)
    {
        String[] splitStr = lineStr.split(",");
        return splitStr;
    }

    public static String getZeroOutput(String lineStr)
    {
        String zeroOutput = splitLineStr(lineStr)[2];
        zeroOutput.trim();
        return zeroOutput;
    }

    public static String remainDecimal(String averageStr)
    {

        return averageStr.substring(0,5);
    }

    public static String getSensitiveData(String[][] sensitiveTable)
    {
        double sum = 0L;
        for(int i = 0;i < 10;i ++)
        {
            String sensitiveData = sensitiveTable[i][4];
            sensitiveData.trim();
            sum = sum + Double.parseDouble(sensitiveData);
        }

        double average = sum / 10;
        String averageStr = String.valueOf(average);
        return remainDecimal(averageStr);
    }

    public static String deleteSpecificChar(String str,String c)
    {
        String newStr = str.replaceAll(c,"");
        return newStr;
    }

    public static String getLinearity(String str)
    {
        String[] linearity = splitLineStr(str);
        String linearityData = linearity[1];
        String rightLinearityData = deleteSpecificChar(linearityData,"%");
        return rightLinearityData;
    }
}
