UsartType.Rx_Flag = 0;
uint8_t reChar;
for(i =0; i < UsartType.size; i++)
{
	reChar = *(UsartType.RX_pData + i);
	if(0xAA == reChar)
	{
		if(3 == gProtocolCmd.HandingFlag)
		{
			gProtocolCmd.HandingFlag = 4;
		}
		if(2 == gProtocolCmd.HandingFlag)
		{
			gProtocolCmd.HandingFlag = 3;
		}
		
	}
	if(0x53 == reChar)
	{
		if(1 == gProtocolCmd.HandingFlag)
		{
			gProtocolCmd.HandingFlag = 2;
		}
		if(0 == gProtocolCmd.HandingFlag)
		{
			gProtocolCmd.HandingFlag = 1;
		}
	}
	
		
	
}