using System;
using System.IO.Ports;
using Microsoft.AspNetCore.Identity;
using System.Text.Json;
using System.Security.Cryptography.X509Certificates;
namespace WebAPI
{
    
    public class HardwareState 
    {
        //DTO, received through printf from arduino
        public int leds { get; set; }
        public int dist { get; set; }
    }

    public class HardwareService
    {
        private SerialPort serialPort;
        
        
        public HardwareState CurrentState { get; private set; } = new HardwareState();

        public HardwareService()
        {
            serialPort = new SerialPort("COM4", 9600);
            serialPort.DataReceived += SerialDataReceived;
            serialPort.Open();
        }

        public void SerialDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try {
                string rawData = serialPort.ReadLine();
                // takes a string from the arduino and transforms it into C# HardwareState
                var data = JsonSerializer.Deserialize<HardwareState>(rawData);
                // it saves info in memory, now any part of the api can just look into CurrentState to see the latest distance without having to directly talk to arduino
                if (data != null) CurrentState = data; 
            }
            catch { }
        }

        public void SendToArduino(string message)
        {
            if (serialPort.IsOpen) serialPort.Write(message);
        }
    }
}
