public class CTV 
{
public:
	CTV(MER_Meas_APPLICATION::Form1^ Handle2Form1);
	friend class CResults;
	friend class CIniWriter;
	friend class CDriver;
	
private:
	gcroot<MER_Meas_APPLICATION::Form1^> form1; //handle to form1
	int TVStandard_SFU;
	int TVStandard_TVAnalyzer;
	int CodeRate;
	int FFTMode; // once a tv obj is created, TV.CodeRate = ...; TV.FFTMode = ...;
	int Constellation;
	int ChannelBW;
	int GuardInt;
	int TVStandard;
	//int InputImpedance;
	//int SidebandPos;
	

public:
	void initialization();
};

CTV::CTV(Form1^ Handle2Form1)
{
	//MER_Meas_APPLICATION::Form1^ form1 ;
	form1 = Handle2Form1; // pass the handle of form1 to the class CTVParam
	TVStandard = form1->comboBox_TVStandard->SelectedIndex;
}

void CTV::initialization()

{
	//TVparam Variables initialization 
	CodeRate = form1->comboBox_CodeRate->SelectedIndex;
	FFTMode = form1->comboBox_FFTMode->SelectedIndex;
	Constellation = form1->comboBox_Constellation->SelectedIndex;
	ChannelBW = form1->comboBox_ChannelBW->SelectedIndex;
	GuardInt = form1->comboBox_GuardInt->SelectedIndex;
    
	// TVstandard initialized
	enum TVStdSelected
	{ 
	  dvbth,
	  isdbt,
	  dvbt2,
	  tvstdmax
	};

	enum SFU_TVStd
	{ 
	  dvbth_sfu= 0,
	  isdbt_sfu = 5,
	  dvbt2_sfu = 16
	};

	enum Analyzer_TVStd
	{
	  dvbth_analyzer = 4,
	  isdbt_analyzer = 7,
	  dvbt2_analyzer = 8,
	};
	
	//Standard = StandardIndex;


	switch(TVStandard)
	{
		
		case dvbth:
			TVStandard_SFU = dvbth_sfu; // the index of dvtbh among the tv std list is 0 in SFU 
			TVStandard_TVAnalyzer = dvbth_analyzer; // the index of dvbth is 4 in ETL, TV analyzer

			break;

		case isdbt:
			TVStandard_SFU = isdbt_sfu;
			TVStandard_TVAnalyzer = isdbt_analyzer;

			break;

		case dvbt2:
			TVStandard_SFU = dvbt2_sfu;
			TVStandard_TVAnalyzer = dvbt2_analyzer;

			break;

		default:
			break;
	}//switch (), declare all the setting parameter variables with values (run time declaration)
	//
}
# IndustrialAttachment2012
