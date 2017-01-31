public class CGeneralData
{
public:
	CGeneralData(MER_Meas_APPLICATION::Form1^ Handle2Form1);
	//int GetMinFreq(void);
	//int GetMaxFreq(void);
	//int GetStepSize(void);
	friend class CResults;
	friend class CIniWriter;
	friend class CDriver;
	//friend class CCommunication;
private:
	gcroot<MER_Meas_APPLICATION::Form1^> form1;
	double MinFreq ;
	double MaxFreq; 
	double StepSize;
	double Delay;
	double Level;
	int Preselection;

};

CGeneralData::CGeneralData(Form1^ Handle2Form1)
{
	//MER_Meas_APPLICATION::Form1^ form1;
    form1 = Handle2Form1;
	MinFreq = (double)form1->numericUpDown_MinFreq->Value;
	MaxFreq = (double)form1->numericUpDown_MaxFreq->Value;
	StepSize = (double)form1->numericUpDown_StepSize->Value;
	Delay = (double)form1->numericUpDown_Delay->Value;//all the gen params can be accessed by CResults, CDriver..
	Level = (double)form1->numericUpDown_Level->Value;
	Preselection = form1->comboBox_Preselection->SelectedIndex;
	
	
}
