public class CResults
{
public:
	CResults();
	bool InitResults(CGeneralData& GeneralData);
	//~CResults();
	friend class CDriver;
	
private:
	//int NoofMER = 10;
	double* freq ; //= new int[100];
	double* MERaverage ;// = new double[100];
	double* MERminimum ;
	double* MERmedian ;
	int TVstd;
	double MER[NOOFMER];  // 10 readings of MER per frequency, 1s delay between each reading, this array will be refreshed for each MER at each single frequency ...
	gcroot<MER_Meas_APPLICATION::Form1^> form1;
	//string apppath;

public:
	void SetMER(int j, double val);
	string FileNameTime;
	int size ;
	void CalculateMER(int i);
	//void CreateResultsFile();// default directory with file name as DateTime
	void WriteTV(CTV TV); // write the TVparams to the filename.txt, with format UI.text = UI.selectedindex
	void WriteGeneralData(CGeneralData GeneralData); // write the min,max frequency, and step size, delay to filename.txt
	void WriteMeasResults(int i); // write the MER[i] for each freq[i] into filename.txt after each reading from the device
	bool createResultsfile(MER_Meas_APPLICATION::Form1^ Handle2Form1);
	void mem_freeup();
};

CResults::CResults()
{
	freq = 0; 
	MERaverage = 0;
	MERminimum = 0;
	MERmedian = 0;
}



bool CResults::InitResults(CGeneralData& GeneralData)
{



	// calculate the size of freq/MER array

	// variable n is used to check whether the difference between max and min freq is a whole number of the step size
	double n;

	n = (GeneralData.MaxFreq - GeneralData.MinFreq) / GeneralData.StepSize ;

	if( (n - int(n))!=0)
	{
		size = (int)n + 2;
	} // n is not an integer, size is (int)n + 2
	else 
	{
		size = (int)n + 1;
	} // n is an integer, size is (int)n + 1 

    // allocate the freq/MER array with size 

    freq = new (nothrow) double [size];
	MERaverage = new (nothrow) double [size];
	MERmedian = new (nothrow) double [size];
	MERminimum = new (nothrow) double [size];

    
	// assign values to each freq[i]
	if (freq == 0 || MERaverage == 0 || MERmedian == 0 || MERminimum == 0)
	{
		return false; // if cannot allocate memory, return false. and delete all allocated memory
	}
	else
	{
		int i = 0;
	    freq[i] = GeneralData.MinFreq;

		while (freq[i] <  GeneralData.MaxFreq)
		{
			freq[i+1] = freq[i] + GeneralData.StepSize;
			i++;
		}
	    
		freq [i] = GeneralData.MaxFreq;

		return true;
	}

} // size of array calculated and freq[size],MERaverage[size] allocated, freq[]initialized 


////CResults::~CResults()
////{
////	// should also do the same thing as mem_freeup so that if the user suddenly shut down the program
////	// objs can be destructed instead of still takin up memory 
////	// this destructor is implicitly called when program shuts down
////
////
////
////}
void CResults::mem_freeup()
{
	// deallocated pointers

	// the array pointers are the only variables holding the address of the dynamically allocated doubles
	// these pointers need to be deleted in order to free up the memory to be reused or reallocated
	// make more memory available to this program and all other programs

	delete [] freq;
	freq = 0; // assign the deallocated pointers to zero to ensure the program does not try and access memory that has been deallocated
	delete [] MERaverage;
	MERaverage = 0;
	delete [] MERmedian;
	MERmedian = 0;
	delete [] MERminimum;
	MERminimum = 0;

}

bool CResults::createResultsfile(Form1^ Handle2Form1)
{
	   form1 = Handle2Form1;
	   time_t now;

	   char filetime[40];
	   char MeasTime[40];
	   
	   filetime[0] = '\0';
	   
	   now = time(NULL);

	   if (now != -1)
	   {
		  strftime(filetime, 40, "MER_%H-%M-%S-%d-%m-%Y.txt", localtime(&now));
		  strftime(MeasTime, 40, "time %H:%M:%S date %d/%m/%Y", localtime(&now)); 
		  // get the current time at the moment of creating results file
		  // MeasTime is a member of CResults, initialized in this method, to be used when later writing into the file
	   }

	   String^ FileName = gcnew String(filetime);
	   String^ CombinePath = Path::Combine(form1->folderbrowserdialog->SelectedPath, FileName); 
	   // combined the selectedpath from the folderbroswedlg with the filename (timedate string)

	   MarshalString(CombinePath, FileNameTime); 
	   // initialize the CResults member FileNameTime of string type
	   
	   fstream ResultsFile;  
	   ResultsFile.open(&FileNameTime[0], std::ios::out);
       // must open the file here so that the file with name FileNameTime is created and exists when check access(,)
	   // file stream automatically closed when it goes beyond the current block

	   if ( _access( &FileNameTime[0], 0) == -1)
	   {
		   return false; 
		   // the saving path is a file path instead of a folder path, cannot be checked when only look at the selectedpath validity
		   // it produces an invalid path after combining with the datetime string, thus return false

	   }

       // file has been created, write the date time string MeasTime into the results file
	   ResultsFile << MeasTime << ";\n" ;

	   return true;
	 


}


void CResults::WriteTV(CTV TV)
{
    // write the tv params into the results file
    string os;
	fstream outTVparam;
	outTVparam.open(&FileNameTime[0], std::ios::out | std::ios::app);// the results file has been created, first time open stream
	
	if (outTVparam.is_open())
	{
		//outTVparam << MeasTime << ";\n" ;
		outTVparam << "TV standard: (" << TV.TVStandard << ") " << MarshalString(TV.form1->comboBox_TVStandard->SelectedItem->ToString(), os)<<"\n";
		outTVparam << "Constellation: (" << TV.Constellation << ") " << MarshalString(TV.form1->comboBox_Constellation->SelectedItem->ToString(), os)<<"\n";
		outTVparam << "Code Rate: (" << TV.CodeRate << ") " << MarshalString(TV.form1->comboBox_CodeRate->SelectedItem->ToString(), os)<<"\n";
		outTVparam << "FFT Mode: (" << TV.FFTMode << ") " << MarshalString(TV.form1->comboBox_FFTMode->SelectedItem->ToString(), os)<<"\n";
		outTVparam << "Channel BandWidth: (" << TV.ChannelBW << ") " << MarshalString(TV.form1->comboBox_ChannelBW->SelectedItem->ToString(), os)<<"\n";
		outTVparam << "Guard Interval: (" << TV.GuardInt << ") " << MarshalString(TV.form1->comboBox_GuardInt->SelectedItem->ToString(), os)<<"\n";

	}


}

void CResults::WriteGeneralData(CGeneralData GeneralData)
{
	// append the gen param information to the results file after the TVParams
    string os;
	fstream outGendata(&FileNameTime[0], std::ios::out | std::ios::app);

	if(outGendata.is_open())
	{
		outGendata << "Minimum frequency : " << (double)GeneralData.MinFreq << "\n";// write "min.freq = " + MinFreq , 
		outGendata << "Maximum frequency : " << (double)GeneralData.MaxFreq << "\n";// max.freq = 
		outGendata << "Step Size : " << (double)GeneralData.StepSize << "\n";// stepsize = 
		outGendata << "Level : " << (double)GeneralData.Level << "\n";// Level = 
		outGendata << "Preselection : " << GeneralData.Preselection << "\n";
		outGendata << "Delay : " << (double)GeneralData.Delay;
	}

}

void CResults::CalculateMER(int i)
{

	int j ;
	double sum = 0.0;
	for (j = 0; j < NOOFMER; j++)
	{
		sum = sum + MER[j];
	}
	MERaverage[i] = sum / NOOFMER ; // get the average from MER[0] to MER[NOOFMER]
    
    if( ( NOOFMER % 2 ) == 0 ) // if the no. of reading NOOFMER is even, then there are two median values
	{
		int MID = NOOFMER / 2;
		nth_element(MER, MER + MID - 1, MER + NOOFMER);
		double firstmed = MER[MID - 1]; // the 5th element if NOOFMER = 10
		nth_element(MER, MER + MID, MER + NOOFMER);
		double secmed = MER[MID]; // the 6th element if NOOFMER = 10
 		MERmedian[i] = ( firstmed + secmed ) / 2; // median is the average of the two median values if the total no. is even
	}

	else // if the no. of reading NOOFMER is odd, only one median.
	{
		int MID = NOOFMER / 2;
		nth_element(MER, MER + MID, MER + NOOFMER);
		MERmedian[i] = MER[MID];
	}


	nth_element(MER, MER + 0, MER + NOOFMER);
	MERminimum[i] = MER[0]; // get the minimum of MER[0]to MER[9]


	//

	//MERmedian[i]= ;
	//MERminimum[i]= ;
	// for each freq[i], call calculateMER(int i)
	// after reading each group of MER, call the CalucalteMER method...when reading mer from device, assign value to MER[j]
	// read 10 MERs for each single frequency, calculate the average, median and minimum of the MERs
	// when using the CCommunication class, only assign the 10 values to MERs...no calculation involved? 
	// MERaverage[i] = ; MERminimum[i] = ; MERmedian[i] = ;
}
void CResults::SetMER(int j, double val)
{
	MER[j] = val; 

}
void CResults::WriteMeasResults(int i)
{

   fstream outMeasResults(&FileNameTime[0], std::ios::out | std::ios::app);

   if(outMeasResults.is_open())
   {
	   outMeasResults << "freq[" << i << "]= " << freq[i] << "MHz;";
	   
	   outMeasResults << "MERaverage[" << i << "]= " << MERaverage[i] << "; "   ; 
	   outMeasResults << "MERmedian[" << i << "]= " << MERmedian[i] << "; " ;
	   outMeasResults << "MERminimum[" << i << "]= " << MERminimum[i] << ";\n" ; // all MER avr,min,med calculated by the 
       
		   if (outMeasResults.bad())
		   {
			   outMeasResults.close();
			   throw gcnew System::Exception("failed to write measurement results");
		   }

	   outMeasResults.close();
   }

}

