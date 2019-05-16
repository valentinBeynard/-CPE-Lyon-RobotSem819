package serial;
import java.io.*;
import gnu.io.*;
public class SerialReceiver{  
	public static void readPort(String portname)   { 
		// args[0] : nom symbolique du port 
		CommPortIdentifier portID = null;  
		try {portID = CommPortIdentifier.getPortIdentifier(portname);}    
		catch (NoSuchPortException e1) {} 
		SerialPort port = null;   
		try {port = (SerialPort) portID.open("SerialReceiver", 2000);} 
		catch (PortInUseException e2) {}   
		try     {  
			port.setSerialPortParams(9600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
			port.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);  
			}     
		catch (UnsupportedCommOperationException e3) {}
		try     {       
			BufferedReader br = new BufferedReader(new InputStreamReader(port.getInputStream(), "US-ASCII"));       System.out.println(br.readLine());     } 
		catch (IOException e4) {}
	}
}

