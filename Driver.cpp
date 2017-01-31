class CDriver
{
public:
	//CDriver();
	//~CDriver();
	bool initDriver(DeviceAddrSetting::Form2^ Handle2Form2, CGeneralData GeneralData);
	void ReadMER(CResults& Results);
	void dummyReadMER(CResults& Results, int i);
	////void ConfigTVParams(CTV& TV);
	////void ConfigFreq(CGeneralData& GeneralData);
	//void AdjustLevel();
	//void ReadTest(CTest& Test);
	bool is_stop;
    void Close();
	void ConfigTVParam(CTV TV, CGeneralData GeneralData);
	void AdjustLevel(CResults Results, int i);
   	void cleanup (ViBoolean deviceClear);
private:
	gcroot<MER_Meas_APPLICATION::Form1^> form1;
	int MERdelay;
	double Level_SFU;
	double Level_TVAnalyzer;
	double Level;// desired level
	ViStatus rssfu_status;
	ViStatus rsetl_status;
	gcroot<DeviceAddrSetting::Form2^> form2;

	//ViRsrc rssfu_RsName; // no need 
	//ViRsrc rsetl_RsName; // can just pass the string into init function
public:
	
	ViSession io_RSSFU; // need to be passed to stopbutton-clicked, 
	ViSession io_TVAnalyzer; // need to be passed to stopbutton-clicked
	char* transmitter_Addr; // will be formatted with "GPIB::" + form2->textbox.text, need handle of form2 passed to this class..
	char* analyzer_Addr; // will be initialized in constructor 
	
};
#define CHECKERR(fcal) if ((rssfu_status = fcal) < 0 || (rsetl_status = fcal) < 0 )\
					   {\
							cleanup(VI_FALSE);\
							return;\
					   }




bool CDriver::initDriver(DeviceAddrSetting::Form2^ Handle2Form2, CGeneralData GeneralData)
{
	is_stop = false;

	form2 = Handle2Form2;
	//form1 = Handle2Form1; 

	// allocated memory of fixed size(max) for the 2 address strings
	transmitter_Addr = new char [50]; 
	analyzer_Addr = new char [50];
	
	// initialize the MERdelay parameter
	MERdelay = GeneralData.Delay;

	Level = GeneralData.Level;
	Level_SFU = GeneralData.Level;

	// initialize the two address strings 
	// check the communication interfaces with the two devices first
	Net::IPAddress^ IP;
	if( form2->RB_Transmitter_GPIB->Checked == true && form2->RB_Analyzer_IP->Checked == true )
	{
		// the textbox containing ip addr need to be try parsed here so that the addr string is initialized with the correct format of ip addr
		// this cannot be done at the property method, which is used for inireader/writer to access, the inifile saves the initial format of ip addr
		//////////////// check abt how ip addr converts betw formats??????
		Net::IPAddress::TryParse(gcnew String ( &form2->AnalyzerAddrText::get()[0]), IP);
		string ip;
		MarshalString(IP->ToString(), ip);	// conversion betw String^ and string

		sprintf(transmitter_Addr, "GPIB::%s::INSTR", & (form2->TransmitterAddrText::get())[0]);		
		sprintf(analyzer_Addr, "TCPIP::%s::INSTR", &ip[0]);
		
	}
	else if ( form2->RB_Transmitter_IP->Checked == true && form2->RB_Analyzer_GPIB->Checked == true  )
	{
        // tryparse the ip textbox text to get the standard format of ip addr string and convert from String^ to string, so that sprintf() can do the string combination work 
		Net::IPAddress::TryParse(gcnew String (& form2->TransmitterAddrText::get()[0]), IP);
		string ip;
		MarshalString(IP->ToString(), ip);

 		sprintf(transmitter_Addr, "TCPIP::%s::INSTR", &ip[0]);
		sprintf(analyzer_Addr, "GPIB::%s::INSTR", & (form2->AnalyzerAddrText::get())[0]);

	}



	else 
	{
		return false;
		// the transmitter and analyzer have the same type of comm interface
		// both are using GPIB or TCP/IP, thus return false, driver session initialization failed
	}
    
	// use the high level attribute based functions to create a new instrument driver session

	 rssfu_status = rssfu_init(transmitter_Addr, 1, 1, &io_RSSFU);	
	if (rssfu_status < 0 )
	{
		 return false; // cannot open sfu session, provide err msg to user???
	}

	rsetl_status = rsetl_init(analyzer_Addr, 1, 1, &io_TVAnalyzer);
	
	if (rsetl_status < 0)
	{
	     return false; // cannot open elt session, provide err msg to user???
	}

	return true;
}

////CDriver::~CDriver()
////{
////
////
////}



void CDriver::ConfigTVParam(CTV TV, CGeneralData GeneralData)
{
    // CONFIG DIGTIAL TV MEAS MODE ON THE ANALYZER
    CHECKERR( rsetl_ConfigureCATVDMeasurementMode (io_TVAnalyzer) );
	//TVstd = TV.TVStandard;

	// CONFIG TV STANDARD ON SFU and tv analyzer
	CHECKERR( rssfu_confTxModulation(io_RSSFU, 0, 0, TV.TVStandard_SFU, 0) );
	//CHECKERR( rssfu_confTxModulation(io_RSSFU, 0, 0, TV.TVStandard_SFU, 0) );

	// CONFIG TV params on sfu and tv analyzer
	switch (TV.TVStandard)
	{
	case '0':
		CHECKERR( rssfu_confDVBTCoding(io_RSSFU, TV.ChannelBW, 5.0e6, TV.FFTMode, TV.GuardInt, 0, TV.Constellation) );
		CHECKERR( rssfu_confDVBTCodeRate(io_RSSFU, 0, TV.CodeRate) );
	case '1':
		CHECKERR( rssfu_confISDBTLayerCoding(io_RSSFU, 6, 0, TV.Constellation, TV.CodeRate, 0) );
		CHECKERR( rssfu_confISDBTCoding (io_RSSFU, TV.FFTMode, TV.GuardInt, 0) );
		CHECKERR( rssfu_ConfigureISDBTChannelBandwidth (io_RSSFU, TV.ChannelBW) );
	case '2':
		CHECKERR( rssfu_confDVBT2BICMParameters (io_RSSFU, 1, 0, TV.CodeRate, TV.Constellation, 0) );
		CHECKERR( rssfu_confDVBT2BICMFramingOFDMParameters (io_RSSFU, TV.ChannelBW, TV.FFTMode, TV.GuardInt, 6) );

	}

    // configure preseletion
	CHECKERR( rsetl_ConfigureCATVPreselectionState(io_TVAnalyzer, GeneralData.Preselection) );
	
}

void CDriver::AdjustLevel(CResults Results, int i)
{
	
	// configure frequency on both sides
	CHECKERR (rssfu_confTxFreq (io_RSSFU, Results.freq[i], 0.0));
	CHECKERR (rsetl_ConfigureCATVChannelFrequency (io_TVAnalyzer, Results.freq[i], RSETL_VAL_ABS));
   
	// select overview measurements
	CHECKERR (rsetl_ConfigureCATVDMeasurement (io_TVAnalyzer, RSETL_VAL_DTV_MEAS_OVER));
	
	// set power level on sfu
	CHECKERR (rssfu_confTxLevel (io_RSSFU, Level_SFU, VI_FALSE , 0.0, 20.0,
							 RSSFU_RFOUT_AMODE_AUTO, RSSFU_VAL_POW_LEV_DBM));
	// query power level from analyzer
    CHECKERR (rsetl_QueryCATVDOverviewResult (io_TVAnalyzer, RSETL_VAL_DTV_RES_LEV, &Level_TVAnalyzer, NULL));
	
	// if the difference between the level read from the analyzer and the desired value is not within a certain limit
	// increment/decrement the level setting on sfu
	// to compensate for the loss of transmission line
	while ( abs( Level_TVAnalyzer - Level) <= 0.2 )
	{	
        Level_SFU =  Level_SFU + (Level - Level_TVAnalyzer);
		CHECKERR (rssfu_confTxLevel (io_RSSFU, Level_SFU, VI_FALSE , 0.0, 20.0, RSSFU_RFOUT_AMODE_AUTO, RSSFU_VAL_POW_LEV_DBM));	
        CHECKERR (rsetl_QueryCATVDOverviewResult (io_TVAnalyzer, RSETL_VAL_DTV_RES_LEV, &Level_TVAnalyzer, NULL));
	}
    // now the difference betw level_tvanalyzer and level is within 0.2 dB
	// now read the MER value from tvanalyzer
    
		
	
}

void CDriver::dummyReadMER(CResults& Results, int i)
{
	int j;
	
	for(j = 0; j < NOOFMER; j++)
	{   
		double MERval;
		
		MERval = ((double)(j+1)*(j+3)/(j+2)+ pow((double)i, 0.1) + 40); // dummy value, ReadMER from device, assign to MERvalue, 
		
		//CHECKERR ( rsetl_QueryCATVDOverviewResult (io_TVAnalyzer, RSETL_VAL_DTV_RES_MERR, &MERval, NULL)); 
		
		Results.SetMER(j,MERval); // pass MERvalue and the index of MER array to the setMER method
        
		System::Threading::Thread::Sleep(MERdelay);

		if(is_stop == true)
		    return ;
		
        
	} // assign values to MER[j], for testing
}


void CDriver::ReadMER(CResults& Results)
{

	int j;
	
	for(j = 0; j < NOOFMER; j++)
	{   
		double MERval;
		
		//MERvalue = ((double)(j+1)*(j+3)/(j+2)+ pow((double)i, 0.1) + 40); // dummy value, ReadMER from device, assign to MERvalue, 
		
		CHECKERR ( rsetl_QueryCATVDOverviewResult (io_TVAnalyzer, RSETL_VAL_DTV_RES_MERR, &MERval, NULL)); 
		
		Results.SetMER(j,MERval); // pass MERvalue and the index of MER array to the setMER method
        
		System::Threading::Thread::Sleep(MERdelay);

		if(is_stop == true)
		    return ;
		
        
	} // assign values to MER[j], for testing

	
} // read MER from TV analyzer and assign values to MER[] for each freq[i]


void CDriver::Close()
{
	delete [] transmitter_Addr;
	transmitter_Addr = 0;
    delete [] analyzer_Addr;
    analyzer_Addr = 0;
	// only close the io session when it is open, USING THE CHECKERR
	/*rssfu_close(io_RSSFU);*/
	// rsetl_close(io_TVAnalyzer);
}


void CDriver::cleanup(ViBoolean deviceClear)
{
	if (rsetl_status < 0)
	{
		ViChar      error_message[256];
		ViStatus    temp_status;
		String^		message;
		String^		error_string;
		
		/* Query the error message from the instrument driver. This
		 * function gets the error message for VISA and driver failures
		 */
		temp_status = rsetl_error_message(io_TVAnalyzer, rsetl_status, error_message);
		
		if (temp_status == VI_WARN_UNKNOWN_STATUS)
		{
			/* Query the error message from the instrument. This
			 * function gets the error message directly from instrument, like
			 * execution error, out of range error etc.
			 */
			rsetl_error_query(io_TVAnalyzer, &temp_status, error_message);
		}
		error_string = gcnew String( error_message );
		message->Format ("Error 0x%8X occured:\n\n%s" , rsetl_status, error_string);
		MessageBox::Show(message, "Instrument Status Error", MessageBoxButtons::OK, MessageBoxIcon::Stop);
		//AfxMessageBox (message, MB_OK | MB_ICONSTOP); 
	}

	if (rssfu_status < 0)
	{
		ViChar      error_message[256];
		ViStatus    temp_status;
		String^		message;
		String^		error_string;
		
		/* Query the error message from the instrument driver. This
		 * function gets the error message for VISA and driver failures
		 */
		temp_status = rssfu_error_message(io_RSSFU, rssfu_status, error_message);
		
		if (temp_status == VI_WARN_UNKNOWN_STATUS)
		{
			/* Query the error message from the instrument. This
			 * function gets the error message directly from instrument, like
			 * execution error, out of range error etc.
			 */
			rssfu_error_query(io_RSSFU, &temp_status, error_message);
		}

		error_string = gcnew String (error_message);
		message->Format ( "Error 0x%8X occured:\n\n%s", rssfu_status, error_string);
		MessageBox::Show( message, "Instrument Status Error", MessageBoxButtons::OK, MessageBoxIcon::Stop );
		//AfxMessageBox (message, MB_OK | MB_ICONSTOP); 
	}
	
	/* Device Clear is a low-level command and should be used in case the
	 * instrument is waiting for Operation Complete to cancel the wait.
	 * It is useful for instance in case of incorrect external trigger
	 * when the instrument does not respond to any other command because of
	 * waiting for trigger.
	 */

	if (deviceClear)
	{
		viClear (io_TVAnalyzer);
        viClear (io_RSSFU);
		viPrintf (io_TVAnalyzer, "*CLS\n");
		viPrintf (io_RSSFU, "*CLS\n");
	} 
	
	rsetl_close (io_TVAnalyzer);
	rssfu_close (io_RSSFU);

	//HCURSOR cur = LoadCursor ( NULL, IDC_ARROW );  
	//SetCursor (cur);

}
