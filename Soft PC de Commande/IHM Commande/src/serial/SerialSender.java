package serial;
import java.io.*;
import gnu.io.*;

public class SerialSender{   
	public static void send(String portname, String message)   {  
		CommPortIdentifier portID = null;
		try {portID = CommPortIdentifier.getPortIdentifier(portname);} 
		catch (NoSuchPortException e1) {}  
		SerialPort port = null;    
		try {
			port = (SerialPort) portID.open("SerialSender", 2000);
			}  
		catch (PortInUseException e2) {
			
		}   
		try     { 
			port.setSerialPortParams(9600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1,SerialPort.PARITY_NONE); 
			port.setFlowControlMode(SerialPort.FLOWCONTROL_NONE); 
			}     
		catch (UnsupportedCommOperationException e3) {
			}     
		try {
			port.getOutputStream().write((message+"\n").getBytes("US-ASCII"));}   
		catch (IOException e4) {}   
		} 
	}
		

