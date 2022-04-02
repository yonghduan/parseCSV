import javax.swing.*;
import java.io.*;
import java.util.ArrayList;

public class ExtractData {

    private File originFile;
    private File generatedFile;
    private BufferedReader bufferedReader;
    private BufferedWriter bufferedWriter;

    private int loopCount = 0;
    private boolean loopStart = false;
    private boolean recordSensitiveFinished = false;
    private boolean recordLinearityFinished = false;

    private static final String[] zeroDetectFlag = {"Limit Lo","Limit Hi"};
    private static final String[] sensitiveDetectFlag = {"Sensitive","Vzero"};
    private static final String[] linearityDetectFlag = {"Vcalc","Delta"};
    private static final String[] accuracyDetectFlag = {"V1","V2"};


    public void reset()
    {

        loopStart = false;
        recordSensitiveFinished = false;
        recordLinearityFinished = false;
    }

    public ExtractData()
    {
        this.originFile = null;
        this.generatedFile = null;
        bufferedReader = null;
        bufferedWriter = null;
    }

    public void chooseFile()
    {
        JFileChooser jFileChooser = new JFileChooser();
        jFileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
        jFileChooser.showDialog(new JLabel(),"请选择一个文件");
        originFile = jFileChooser.getSelectedFile();
    }

    public boolean isSelectedFileRightly()
    {
        String fileName = originFile.getName();
        fileName.trim();
        String[] splitStr = fileName.split("\\.");
        if(splitStr[1].equals("csv"))
            return true;
        else
            return false;
    }

    public void setGeneratedFile()
    {
        String fileName = originFile.getName();
        String path = originFile.getParent();
        generatedFile = new File(path + "\\generated-" + fileName);
    }

    public void readData()
    {
        DataInputStream dataInputStream = null;

        try
        {
            dataInputStream = new DataInputStream(new FileInputStream(originFile));
            bufferedReader = new BufferedReader(new InputStreamReader(dataInputStream,"UTF-16"));
        }
        catch (FileNotFoundException fileNotFoundException)
        {
            throw new RuntimeException(fileNotFoundException.getCause());
        }
        catch (UnsupportedEncodingException unsupportedEncodingException)
        {
            throw new RuntimeException(unsupportedEncodingException.getCause());
        }

    }

    public void setBufferedWriter() {
        setGeneratedFile();
        try
        {
            this.bufferedWriter = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(generatedFile),"UTF-16"));
        }
        catch (FileNotFoundException e)
        {
            throw new RuntimeException(e.getCause());
        }
        catch (UnsupportedEncodingException unsupportedEncodingException)
        {
            throw new RuntimeException(unsupportedEncodingException.getCause());
        }
    }

    public void parseData()
    {
        readData();
        setBufferedWriter();
        String lineStr = null;
        try
        {
            String str = "序号,零点输出,灵敏度,线性度正向,线性度负向,精度";
            bufferedWriter.write(str);
            bufferedWriter.newLine();
        }
        catch(IOException e)
        {
            throw new RuntimeException(e.getCause());
        }

        ArrayList<String> arrayList = new ArrayList<>();
        for(int i = 0;i < 6;i ++)
         arrayList.add("");

        try {
            while ((lineStr = bufferedReader.readLine()) != null)
            {
                if(DetectUtils.detectChildrenString(lineStr,zeroDetectFlag) && !loopStart &&
                        !DetectUtils.detectChildrenString(lineStr,sensitiveDetectFlag))
                {
                    loopStart = true;
                    loopCount ++;

                    arrayList.set(0,String.valueOf(loopCount) + ",");

                    lineStr = bufferedReader.readLine();
                    arrayList.set(1,DetectUtils.getZeroOutput(lineStr) + ",");
                    System.out.println("loop" + loopCount + "zeroOutput finished");
                    continue;
                }

                if(DetectUtils.detectChildrenString(lineStr,sensitiveDetectFlag) && loopStart)
                {
                    /**
                     * get ten data*/
                    String[][]  sensitiveTable = new String[10][8];
                    for(int i = 0;i < 10; i ++)
                    {
                        lineStr = bufferedReader.readLine();
                        String[] sensitiveLine = DetectUtils.splitLineStr(lineStr);
                        sensitiveTable[i] = sensitiveLine;
                    }
                    String sensitiveAverage = DetectUtils.getSensitiveData(sensitiveTable);
                    arrayList.set(2,sensitiveAverage + ",");
                    recordSensitiveFinished = true;
                    System.out.println( "loop " + loopCount + ": sensitive detect finished");

                    continue;
                }

                if(DetectUtils.detectChildrenString(lineStr,linearityDetectFlag) && recordSensitiveFinished)
                {
                    for(int i = 0;i < 13;i ++)
                    {
                        lineStr = bufferedReader.readLine();
                    }
                    if(!DetectUtils.detectContinuousField(lineStr,"Vout"))
                        throw new RuntimeException("detect continuous word 线性度 error");
                    lineStr = bufferedReader.readLine();


                    arrayList.set(3,DetectUtils.getLinearity(lineStr) + ",");

                    System.out.println("loop " + loopCount + ": linearity positive detect finished");

                    for(int i = 0;i < 13;i ++)
                        lineStr = bufferedReader.readLine();
                    if(!DetectUtils.detectContinuousField(lineStr,"Vout"))
                        throw new RuntimeException("detect continuous word 线性度(negative) error");
                    lineStr = bufferedReader.readLine();

                    arrayList.set(4,DetectUtils.getLinearity(lineStr) + ",");

                    recordLinearityFinished = true;
                    System.out.println("loop " + loopCount + ": linearity negative detect finished");

                    continue;

                }

                if(DetectUtils.detectChildrenString(lineStr,accuracyDetectFlag) && recordLinearityFinished)
                {
                    for(int i = 0; i < 23;i ++)
                        lineStr = bufferedReader.readLine();
                    if(!DetectUtils.detectContinuousField(lineStr,"Vout"))
                        throw new RuntimeException("detect continuous word 精度 error");

                    lineStr = bufferedReader.readLine();
                    arrayList.set(5,DetectUtils.getLinearity(lineStr));
                    StringBuilder stringBuilder = new StringBuilder();
                    for(int i = 0;i < 6;i ++)
                    {
                        stringBuilder.append(arrayList.get(i));
                    }
                    bufferedWriter.write(stringBuilder.toString());
                    bufferedWriter.newLine();

                    reset();
                    System.out.println("loop " + loopCount + ": accuracy detect finished");

                    continue;
                }
            }
        }
        catch(IOException e)
        {
            throw new RuntimeException(e.getCause());
        }
    }

    public void close()
    {
       try
       {
           bufferedReader.close();
           bufferedWriter.flush();
           bufferedWriter.close();
       }
       catch (IOException e)
       {
           throw new RuntimeException(e.getCause());
       }
    }

    public void run()
    {
        chooseFile();
        if(! isSelectedFileRightly())
            throw new RuntimeException("file is not a csv file");

        setGeneratedFile();
        parseData();
        close();
    }

    public static void start()
    {
        ExtractData extractData = new ExtractData();
        extractData.run();

    }

}
