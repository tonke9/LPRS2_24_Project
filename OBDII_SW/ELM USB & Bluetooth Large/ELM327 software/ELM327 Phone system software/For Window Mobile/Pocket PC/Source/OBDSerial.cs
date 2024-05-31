// OBD Gauge
// Copyright (C) 2005 Dana Peters

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

using System;
using System.Text;
using System.Collections;
using OpenNETCF.IO.Serial;

namespace OBDGauge
{
	/// <summary>
	/// class to read and write serial data
	/// </summary>
	
	public class OBDSerial
	{
		Port mPort;
		Queue mQueue = new Queue();

		static OBDSerial mOBDSerial;

		static public void CreateSingleton()
		{
			mOBDSerial = new OBDSerial();
		}

		static public OBDSerial GetSingleton()
		{
			return mOBDSerial;
		}

		static public void DestroySingleton()
		{
			mOBDSerial = null;
		}

		private OBDSerial()
		{
		}

		public void SerialOpen(String portName, BaudRates baud)
		{
			BasicPortSettings portSettings = new BasicPortSettings();
			portSettings.BaudRate = baud;
			mPort = new Port(portName, portSettings);
			mPort.RThreshold = 1;
			mPort.SThreshold = 1;	// send 1 byte at a time
			mPort.InputLen = 0;
			mPort.Open();
			mPort.DataReceived +=new Port.CommEvent(mPort_DataReceived);
		}

		public void SerialClose()
		{
			if (mPort != null && mPort.IsOpen)
			{
				mPort.Close();
				mPort.Dispose();
			}
		}

		public int SerialCheck()
		{
			if (mPort != null && mPort.IsOpen && mQueue.Count > 0)
			{
				byte[] inputData = (byte [])mQueue.Peek();
				return inputData.Length;
			}
			return 0;
		}

		public byte[] SerialRead()
		{
			byte[] inputData = (byte [])mQueue.Dequeue();
			return inputData;
		}

		public void SerialWrite(String text)
		{
			SerialWrite(Encoding.ASCII.GetBytes(text), text.Length);
		}

		public void SerialWrite(byte[] buf, int buflen)
		{
			if (mPort != null && mPort.IsOpen)
			{
				byte[] outputData = new byte[buflen];
				Array.Copy(buf, 0, outputData, 0, buflen);
				mPort.Output = outputData;
			}
		}

		private void mPort_DataReceived()
		{
			byte[] inputData = new byte[mPort.InputLen];
			inputData = mPort.Input;
			mQueue.Enqueue(inputData);
		}

	}
}
