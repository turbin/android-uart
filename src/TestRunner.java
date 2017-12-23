import com.turbineyan.devicedelegate.Uart;
import com.turbineyan.devicedelegate.UartConfig;
import java.lang.String;
import java.nio.charset.Charset;
import java.lang.Exception;


class InnerException extends Exception{
    private int mErrorCode;

    public InnerException(int err){
         this.mErrorCode = err;
    }

    @Override
    public String toString() {
     return "Inner Exception[" + this.mErrorCode + "]";
   }
}

public class TestRunner {

    public static void dump_bytes(byte[] in) {
        for(int i=0; i!=in.length; i++) {
            System.out.printf("0x%02x ",in[i]);
        }
        System.out.print("\n");
    }


    public static void main(String args[]) {

        Uart uart = Uart.createInstance(new UartConfig(UartConfig.BaudRate.BAUD19200.getValue(), 
                                                       UartConfig.ParityMode.MODE_8N1.getValue()));
        
        int nRet = uart.open();
        if(0 > nRet) {
            System.out.println("open dev error: " + nRet);
            return;
        }

        final String strEchoMessage = "echo message !";
        byte[] buffer = new byte[strEchoMessage.length()];

        int times = 0;

        try{
            while(true) {

                nRet = uart.send(strEchoMessage.getBytes(Charset.forName("UTF-8")), 5);
                
                if(0 > nRet) {
                    throw new InnerException(nRet);
                }

                nRet = uart.recv(buffer, 5);

                if(0 > nRet) {
                    throw new InnerException(nRet);
                }

                // dump_bytes(buffer);
                
                String feedBack = new String(buffer);
                System.out.println("feedBack: len= " + feedBack.length());
                dump_bytes(feedBack.getBytes(Charset.forName("UTF-8")));
                System.out.println("strEchoMessage:");
                dump_bytes(strEchoMessage.getBytes(Charset.forName("UTF-8")));

                if(!strEchoMessage.equals(feedBack)) {
                    throw new Exception("NOT MATCH!");
                }
                
                System.out.println("recv lenth= "+ buffer.length + "run times= " + ++times);
            }
        }catch(Exception e){
                System.err.println("Catch exception " + e);
        } finally {
            uart.close();
        }
        
    }
}
