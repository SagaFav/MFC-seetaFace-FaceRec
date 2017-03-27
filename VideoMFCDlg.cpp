
// VideoMFCDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VideoMFC.h"
#include "VideoMFCDlg.h"
#include "afxdialogex.h"
#include <cstdint>

#include <string>

#include "SeetaFace.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace cv;

CvCapture* capture;
CRect rect, rect1,rect01,rect02,rect03,rect04,rect05,rect06,rect07,rect08;
CDC *pDC, *pDC1, *pDC01, *pDC02, *pDC03, *pDC04, *pDC05, *pDC06, *pDC07, *pDC08;
HDC hDC, hDC1, hDC01, hDC02, hDC03, hDC04, hDC05, hDC06, hDC07, hDC08;
CWnd *pwnd, *pwnd1, *pwnd01, *pwnd02, *pwnd03, *pwnd04, *pwnd05, *pwnd06, *pwnd07, *pwnd08;

string testName;

//**************************************************************************************
IplImage *resizeRes;//��ż�⵽������
IplImage* faceGray;//��ż�⵽������ �Ҷ�ͼ��
bool bool_cameOpen = false;//ȫ�ֱ��� ��־����ͷ�Ƿ��
bool bool_picNum = false;//ȫ�ֱ��� ��־ѵ��ͼƬ�Ƿ�Ϊ��
bool bool_detec_reco = false;//ȫ�ֱ��� 
double dConfidence = 0;//���Ŷ�
int predictedLabel = 100000;

CvMemStorage* storage = 0;
CvHaarClassifierCascade* cascade = 0;
CvHaarClassifierCascade* nested_cascade = 0;
int use_nested_cascade = 0;
const char* cascade_name =
"../data/haarcascades/haarcascade_frontalface_alt.xml";
const char* nested_cascade_name =
"../data/haarcascade_eye_tree_eyeglasses.xml";
double scale = 1;
int num_components = 9;
double facethreshold = 9.0;
//cv::Ptr<cv::FaceRecognizer> model = cv::createFisherFaceRecognizer();
cv::Ptr<cv::FaceRecognizer> model = cv::createLBPHFaceRecognizer();//LBP����������ڵ���������֤����Ч�����
//cv::Ptr<cv::FaceRecognizer> model = cv::createEigenFaceRecognizer();
vector<Mat> images;
vector<int> labels;

IplImage *frame, *frame_copy = 0;
IplImage *image = 0;
const char* scale_opt = "--scale="; // ������ѡ��ָʾ���� 
int scale_opt_len = (int)strlen(scale_opt);
const char* cascade_opt = "--cascade=";
int cascade_opt_len = (int)strlen(cascade_opt);
const char* nested_cascade_opt = "--nested-cascade";
int nested_cascade_opt_len = (int)strlen(nested_cascade_opt);
int i;
const char* input_name = 0;

// CAboutDlg dialog used for App About
//CString strConfidence = "70";
//CEdit* pEdtConfidence;
CString strTip = NULL;
CEdit* pTip;
//**************************************************************************************

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVideoMFCDlg �Ի���



CVideoMFCDlg::CVideoMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideoMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVideoMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON1, &CVideoMFCDlg::OnClickedButton1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, &CVideoMFCDlg::OnClickedButton2)
	
	ON_BN_CLICKED(IDC_BUTTON3, &CVideoMFCDlg::OnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CVideoMFCDlg::OnBnClickedButton4)
	ON_STN_CLICKED(IDC_NO3, &CVideoMFCDlg::OnStnClickedNo3)
	ON_STN_CLICKED(IDC_NO7, &CVideoMFCDlg::OnStnClickedNo7)
	ON_BN_CLICKED(IDC_BUTTON5, &CVideoMFCDlg::OnBnClickedButton5)
	ON_STN_CLICKED(IDC_STATIC01, &CVideoMFCDlg::OnStnClickedStatic01)
END_MESSAGE_MAP()


// CVideoMFCDlg ��Ϣ�������

BOOL CVideoMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	pTip = (CEdit*)GetDlgItem(IDC_EDIT1);
	//��Ƶ��ʾ����
	pwnd = GetDlgItem(IDC_ShowImage);
	//��ͼ��ʾ����
	pwnd1 = GetDlgItem(IDC_JIEQU);

	//*******************************************
	//ʶ�������
	pwnd01 = GetDlgItem(IDC_NO1);
	pwnd02 = GetDlgItem(IDC_NO2);
	pwnd03 = GetDlgItem(IDC_NO3);
	pwnd04 = GetDlgItem(IDC_NO4);
	pwnd05 = GetDlgItem(IDC_NO5);
	pwnd06 = GetDlgItem(IDC_NO6);
	pwnd07 = GetDlgItem(IDC_NO7);
	pwnd08 = GetDlgItem(IDC_NO8);

	//*******************************************
	
	pDC = pwnd->GetDC();
	pDC1 = pwnd1->GetDC();
	//*******************************************
	pDC01 = pwnd01->GetDC();
	pDC02 = pwnd02->GetDC();
	pDC03 = pwnd03->GetDC();
	pDC04 = pwnd04->GetDC();
	pDC05 = pwnd05->GetDC();
	pDC06 = pwnd06->GetDC();
	pDC07 = pwnd07->GetDC();
	pDC08 = pwnd08->GetDC();

	//*******************************************
	

	hDC = pDC->GetSafeHdc();
	hDC1 = pDC1->GetSafeHdc();

	//*******************************************
	hDC01 = pDC01->GetSafeHdc();
	hDC02 = pDC02->GetSafeHdc();
	hDC03 = pDC03->GetSafeHdc();
	hDC04 = pDC04->GetSafeHdc();
	hDC05 = pDC05->GetSafeHdc();
	hDC06 = pDC06->GetSafeHdc();
	hDC07 = pDC07->GetSafeHdc();
	hDC08 = pDC08->GetSafeHdc();
	//*******************************************

	pwnd->GetClientRect(&rect);
	pwnd1->GetClientRect(&rect1);

	//*******************************************
	pwnd01->GetClientRect(&rect01);
	pwnd02->GetClientRect(&rect02);
	pwnd03->GetClientRect(&rect03);
	pwnd04->GetClientRect(&rect04);
	pwnd05->GetClientRect(&rect05);
	pwnd06->GetClientRect(&rect06);
	pwnd07->GetClientRect(&rect07);
	pwnd08->GetClientRect(&rect08);

	//*******************************************

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CVideoMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CVideoMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CVideoMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//������ͷ
void CVideoMFCDlg::OnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// TODO: Add your control notification handler code here
	//AfxMessageBox("OK");
	if (!capture)
	{
		capture = cvCaptureFromCAM(0);
		//AfxMessageBox("OK");
	}

	if (!capture)
	{
		AfxMessageBox(_T("�޷�������ͷ"));
		return;
	}

	// ����
	IplImage* m_Frame;
	m_Frame = cvQueryFrame(capture);
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame, 1);

	////��ȡ��ȡͼƬ
	//imageCap.CopyOf(m_Frame);

	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
		//cvWaitKey(10);
	}

	// ���ü�ʱ��,ÿ10ms����һ���¼�
	SetTimer(1, 10, NULL);

	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0); // ���ط����� 
	if (!cascade)
	{
		//MessageBox("�޷����ط������ļ�����ȷ�ϣ�");
	}
	storage = cvCreateMemStorage(0); // �����ڴ�洢�� 

	//if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') ) // �ж������������Ƶ��ţ������ļ� 
	capture = cvCaptureFromCAM(!input_name ? 0 : input_name[0] - '0'); // ������Ƶ��ȡ�ṹ 
	/*
	else if( input_name )
	{
	image = cvLoadImage( input_name, 1 ); // �����ͼ�������
	if( !image )
	{
	capture = cvCaptureFromAVI( input_name ); // ����ͼ��������Ƶ��ȡ
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480); ////////////////////////////////////////////////////////////////////
	}
	}*/
	

	GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(true);

	//���հ�ť
	//GetDlgItem(IDC_Photograph)->EnableWindow(true);

	bool_detec_reco = false;
	bool_cameOpen = true;
}



void CVideoMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/************************************************************************/
	/* ��ʾ����ͷ                                                           */
	/************************************************************************/
	IplImage* m_Frame;
	m_Frame = cvQueryFrame(capture);

	//�ж��Ǽ�⻹��ʶ������
	if (bool_cameOpen)
	{
		if (!bool_detec_reco)//falseֻΪʶ��
		{
			detect_and_draw(m_Frame);//�������
			//_cprintf("%s\n", "jiance");
		}
		else if (bool_picNum)//false����ѵ��ͼƬΪ��
			recog_and_draw(m_Frame);//����ʶ������
	}

	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame, 1);
	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
		//cvWriteFrame(writer,m_Frame);
		//cvWaitKey(10);
	}

	CDialogEx::OnTimer(nIDEvent);
}

//�ر�����ͷ
void CVideoMFCDlg::OnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
	cvReleaseCapture(&capture);
	CDC MemDC;
	CBitmap m_Bitmap1;
	m_Bitmap1.LoadBitmap(IDB_BITMAP1);
	MemDC.CreateCompatibleDC(NULL);
	MemDC.SelectObject(&m_Bitmap1);
	pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &MemDC, 0, 0, 48, 48, SRCCOPY);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
	bool_cameOpen = false;
}

//����
void CVideoMFCDlg::OnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!faceGray)
	{
		//pTip->GetWindowText(strTip);
		CString tipPhoto = strTip + "\r\n����ʧ�ܣ��뽫����ͷ��׼����";
		pTip->SetWindowText(tipPhoto);
		return;
	}
	Mat img(faceGray, 0);
	stringstream ss;
	ss << (read_img_number() + 1);
	string faceImgName = "..//data//CapImages//" + ss.str() + ".jpg";
	imwrite(faceImgName, img);

	testName = "..//data//CapImages//" + ss.str() + ".jpg";

	//pTip->GetWindowText(strTip);
	CString tipPhoto = strTip + "\r\n���ճɹ����Ѵ�Ϊ" + ("/data/CapImages/" + ss.str() + ".jpg").c_str();
	pTip->SetWindowText(tipPhoto);

}

//***************************************

Detector::Detector(const char* model_name) : seeta::FaceDetection(model_name){
	this->SetMinFaceSize(40);
	this->SetScoreThresh(2.f);
	this->SetImagePyramidScaleFactor(0.8f);
	this->SetWindowStep(4, 4);
}

SeetaFace::SeetaFace(){
	this->detector = new Detector("model/seeta_fd_frontal_v1.0.bin");
	this->point_detector = new seeta::FaceAlignment("model/seeta_fa_v1.1.bin");
	this->face_recognizer = new seeta::FaceIdentification("model/seeta_fr_v1.0.bin");
}

float* SeetaFace::NewFeatureBuffer(){
	return new float[this->face_recognizer->feature_size()];
}

bool SeetaFace::GetFeature(string filename, float* feat){
	//����ж������������һ����,����true
	//���û���������false
	//read pic greyscale
	//printf("ok");
	cv::Mat src_img = cv::imread(filename, 0);

	seeta::ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
	src_img_data.data = src_img.data;

	//read pic color
	cv::Mat src_img_color = cv::imread(filename, 1);
	seeta::ImageData src_img_data_color(src_img_color.cols, src_img_color.rows, src_img_color.channels());
	src_img_data_color.data = src_img_color.data;

	std::vector<seeta::FaceInfo> faces = this->detector->Detect(src_img_data);
	int32_t face_num = static_cast<int32_t>(faces.size());
	if (face_num == 0)
	{
		return false;
	}
	seeta::FacialLandmark points[5];
	this->point_detector->PointDetectLandmarks(src_img_data, faces[0], points);

	this->face_recognizer->ExtractFeatureWithCrop(src_img_data_color, points, feat);

	return true;
};

int SeetaFace::GetFeatureDims(){
	return this->face_recognizer->feature_size();
}

float SeetaFace::FeatureCompare(float* feat1, float* feat2){
	return this->face_recognizer->CalcSimilarity(feat1, feat2);
}

FeatureGroup::FeatureGroup(int feat_dims, SeetaFace* seeta_face){
	this->feat_dims = feat_dims;
	this->seeta_face = seeta_face;
}

int FeatureGroup::GetFeatureDims(){
	return this->feat_dims;
}

bool FeatureGroup::AddFeature(float* feat, string filename){
	Feature temp;
	float* new_feat = new float[this->feat_dims];
	memcpy(new_feat, feat, sizeof(new_feat)*this->feat_dims);
	temp.data = new_feat;
	temp.filename = filename;
	this->feats.push_back(temp);
	return true;
}

bool FeatureGroup::SaveModel(string model_file){
	std::ofstream file;
	file.open(model_file);
	file << int(this->feats.size()) << std::endl;
	file << this->feat_dims << std::endl;
	for (int i = 0; i<int(this->feats.size()); i++){
		file << this->feats[i].filename << std::endl;
		for (int j = 0; j<this->feat_dims; j++)
			file << this->feats[i].data[j] << " ";
		file << std::endl;
	}
	file.close();
	return true;
}

FeatureGroup::FeatureGroup(string model_file, SeetaFace* seeta_face){
	std::ifstream file;
	file.open(model_file);
	int size;
	float* new_feat;
	char* buffer = new char[1000];
	file >> size;
	file >> this->feat_dims;
	for (int i = 0; i<size; i++){
		Feature temp;
		file.getline(buffer, 1000);
		while (buffer[0] == '\0' || buffer[0] == ' '){
			file.getline(buffer, 1000);
		}
		temp.filename = buffer;
		new_feat = new float[this->feat_dims];
		for (int j = 0; j<this->feat_dims; j++)
			file >> new_feat[j];
		temp.data = new_feat;
		this->feats.push_back(temp);
	}
	file.close();
	this->seeta_face = seeta_face;
}

FeatureGroup::~FeatureGroup(){
	for (int i = 0; i<int(this->feats.size()); i++)
		delete[](this->feats[i].data);
}

bool FeatureGroup::FindTopK(int k, float* feat, std::vector<Feature>& result){
	std::cout << "Calculating Similarities..." << std::endl;
	for (int i = 0; i<int(this->feats.size()); i++){
		this->feats[i].similarity_with_goal = this->seeta_face->FeatureCompare(this->feats[i].data, feat);
	}
	std::cout << "Finding TopK..." << std::endl;
	std::priority_queue<Feature> q;
	for (int i = 0; i<int(this->feats.size()); i++)
		q.push(this->feats[i]);
	for (int i = 0; i<k; i++){
		if (q.empty()) return true;
		result.push_back(q.top());
		q.pop();
	}
	return 0;
}

bool GetFilenameUnderPath(string file_path, std::vector<string>& files){
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(file_path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					GetFilenameUnderPath(p.assign(file_path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				char *ext = strrchr(fileinfo.name, '.');
				if (ext){
					ext++;
					if (_stricmp(ext, "jpg") == 0 || _stricmp(ext, "tif") == 0)
						files.push_back(p.assign(file_path).append("\\").append(fileinfo.name));
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}

//ʶ��
void CVideoMFCDlg::OnBnClickedButton4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	/*int choice;
	std::cout << "Input 1 to Build Database Index, 2 to Test Image:" << std::endl;
	std::cin >> choice;

	if (choice == 1){
		std::vector<string> filenames;
		GetFilenameUnderPath(".\\Accessory", filenames);
		std::cout << "Detected  " << filenames.size() << "  images...." << std::endl;
		SeetaFace sf;
		FeatureGroup feature_group(sf.GetFeatureDims(), &sf);
		float* feat = sf.NewFeatureBuffer();
		for (int i = 0; i<int(filenames.size()); i++){
			if (sf.GetFeature(filenames[i], feat))
				feature_group.AddFeature(feat, filenames[i]);
			if ((i + 1) % 1 == 0)
				std::cout << (i + 1) << " / " << int(filenames.size()) << std::endl;
		}
		feature_group.SaveModel("test1.index");

		std::cout << "Finished." << std::endl;
	}*/
	//else{
		SeetaFace sf;
		string pic_file;
		//std::cout << "Loading Database..." << std::endl;
		FeatureGroup feature_group("lab.index", &sf);
		float* feat = sf.NewFeatureBuffer();
		//while (true){
			std::vector<Feature> result;
			//std::cout << "Please Input Your Filename:  ";
			//std::cin >> pic_file;
			pic_file = testName;
			//pic_file = "A46.tif";
			if (sf.GetFeature(pic_file, feat) == false){
				std::cout << "Wrong Filename or Can't Detect Face.." << std::endl;
				//continue;
			}

			//feature_group.FindTopK(10, feat, result);
			/*for (int i = 0; i < int(result.size()); i++){
				std::cout << "Top " << (i + 1) << " : " << result[i].filename << "  Similarity: " << result[i].similarity_with_goal << std::endl;

			}*/
			feature_group.FindTopK(9, feat, result);
			//1
			if (result[0].similarity_with_goal>0.68)  //���Ŷ�
			{
				IplImage* src_img_color01 = cvLoadImage(result[0].filename.c_str(), 1);

				CvvImage m_CvvImage01;
				m_CvvImage01.CopyOf(src_img_color01);
				m_CvvImage01.DrawToHDC(hDC01, &rect01);

				CString similarity01;
				similarity01.Format(_T("similarity: %f"), result[0].similarity_with_goal);

				GetDlgItem(IDC_STATIC01)->SetWindowTextW(similarity01);

				CString tipPhoto = strTip + "ƥ�䵽���������Ƶ�����ͼ��Ϊ" + result[0].filename.c_str() + "���ƶ�Ϊ��" + similarity01;
				pTip->SetWindowText(tipPhoto);

				//2
				IplImage* src_img_color02 = cvLoadImage(result[1].filename.c_str(), 1);

				CvvImage m_CvvImage02;
				m_CvvImage02.CopyOf(src_img_color02);
				m_CvvImage02.DrawToHDC(hDC02, &rect02);

				CString similarity02;
				similarity02.Format(_T("similarity: %f"), result[1].similarity_with_goal);

				GetDlgItem(IDC_STATIC2)->SetWindowTextW(similarity02);

				//3
				IplImage* src_img_color03 = cvLoadImage(result[2].filename.c_str(), 1);

				CvvImage m_CvvImage03;
				m_CvvImage03.CopyOf(src_img_color03);
				m_CvvImage03.DrawToHDC(hDC03, &rect03);

				CString similarity03;
				similarity03.Format(_T("similarity: %f"), result[2].similarity_with_goal);

				GetDlgItem(IDC_STATIC3)->SetWindowTextW(similarity03);

				//4
				IplImage* src_img_color04 = cvLoadImage(result[3].filename.c_str(), 1);

				CvvImage m_CvvImage04;
				m_CvvImage04.CopyOf(src_img_color04);
				m_CvvImage04.DrawToHDC(hDC04, &rect04);

				CString similarity04;
				similarity04.Format(_T("similarity: %f"), result[3].similarity_with_goal);

				GetDlgItem(IDC_STATIC4)->SetWindowTextW(similarity04);

				//5
				IplImage* src_img_color05 = cvLoadImage(result[4].filename.c_str(), 1);

				CvvImage m_CvvImage05;
				m_CvvImage05.CopyOf(src_img_color05);
				m_CvvImage05.DrawToHDC(hDC05, &rect05);

				CString similarity05;
				similarity05.Format(_T("similarity: %f"), result[4].similarity_with_goal);

				GetDlgItem(IDC_STATIC5)->SetWindowTextW(similarity05);

				//6
				IplImage* src_img_color06 = cvLoadImage(result[5].filename.c_str(), 1);

				CvvImage m_CvvImage06;
				m_CvvImage06.CopyOf(src_img_color06);
				m_CvvImage06.DrawToHDC(hDC06, &rect06);

				CString similarity06;
				similarity06.Format(_T("similarity: %f"), result[5].similarity_with_goal);

				GetDlgItem(IDC_STATIC6)->SetWindowTextW(similarity06);

				//7
				IplImage* src_img_color07 = cvLoadImage(result[6].filename.c_str(), 1);

				CvvImage m_CvvImage07;
				m_CvvImage07.CopyOf(src_img_color07);
				m_CvvImage07.DrawToHDC(hDC07, &rect07);

				CString similarity07;
				similarity07.Format(_T("similarity: %f"), result[6].similarity_with_goal);

				GetDlgItem(IDC_STATIC7)->SetWindowTextW(similarity07);

				//8
				IplImage* src_img_color08 = cvLoadImage(result[7].filename.c_str(), 1);

				CvvImage m_CvvImage08;
				m_CvvImage08.CopyOf(src_img_color08);
				m_CvvImage08.DrawToHDC(hDC08, &rect08);

				CString similarity08;
				similarity08.Format(_T("similarity: %f"), result[7].similarity_with_goal);

				GetDlgItem(IDC_STATIC8)->SetWindowTextW(similarity08);
			}
			else
			{
				CString tipPhoto = strTip + "��������û��������Ƭ��";
				pTip->SetWindowText(tipPhoto);
			}
			
		//}
		//std::cout << std::endl;
	//}
}


void CVideoMFCDlg::OnStnClickedNo3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CVideoMFCDlg::OnStnClickedNo7()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

//��ȡ��Ƶ��Ƭ
void CVideoMFCDlg::OnBnClickedButton5()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!faceGray)
	{
		//pTip->GetWindowText(strTip);
		CString tipPhoto = strTip + "\r\n����ʧ�ܣ��뽫����ͷ��׼����";
		pTip->SetWindowText(tipPhoto);
		return;
	}
	Mat img(faceGray, 0);

	string faceImgName = "test.jpg";
	imwrite(faceImgName, img);

	testName = faceImgName;

	//pTip->GetWindowText(strTip);
	CString tipPhoto = strTip + "\r\n���ճɹ������recognize��ť����ʶ������";
	pTip->SetWindowText(tipPhoto);

	IplImage *src;
	src = &IplImage(img);
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(src);
	m_CvvImage.DrawToHDC(hDC1, &rect1);
}


void CVideoMFCDlg::OnStnClickedStatic01()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
