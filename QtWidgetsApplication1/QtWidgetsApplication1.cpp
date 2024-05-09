#include "QtWidgetsApplication1.h"
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <QMessageBox>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <locale>
#include <shellapi.h>
#include <QFileDialog>
#include "downloader.h"
#include "LoginWidget.h"

extern QString g_DownloadPath;

QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
    : QMainWindow(parent), m_isLogInlevel(-1)
{
	m_loginWidget = new LoginWidget;
	m_loginWidget->setWindowModality(Qt::WindowModality::ApplicationModal);
	m_download = new Downloader(this);
	QVBoxLayout* layout = new QVBoxLayout;
	m_loginButton = new QPushButton("login");
	m_downloadLicenseButton = new QPushButton("download license");
	m_button1 = new QPushButton("update installation package");
	m_button2 = new QPushButton("get result");
	m_button3 = new QPushButton("show resources");
	m_checkbutton = new QPushButton("verify license");
	m_line = new QTextEdit("to update");
	m_line->setReadOnly(true);
	m_resTable = new QTableWidget();
	m_resTable->setRowCount(1);
	m_resTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_button5 = new QPushButton("check overdrive package validity");
	m_button6 = new QPushButton("upload logs");
	m_progressBar = new QProgressBar();
	m_progressBar->setVisible(false);
	m_progressBar->setTextVisible(true);
	m_progressBar->setMaximum(100);
	m_progressBar->setMinimum(0);
	//add scrollBar
	QScrollArea* scrollBar = new QScrollArea();
    m_output = new QLabel();
	m_output->adjustSize();
	m_output->resize(450, 150);
	m_output->setWordWrap(true);
	m_output->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	m_output->setText("result\n");
	scrollBar->setWidget(m_output);
	scrollBar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollBar->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->resize(500,500);
	
	layout->addWidget(m_loginButton);
	//layout->addWidget(m_button2);
	layout->addWidget(m_button3);
	//layout->addWidget(m_line);
	layout->addWidget(m_resTable);
	layout->addWidget(m_button1);
	layout->addWidget(m_downloadLicenseButton);
	layout->addWidget(m_checkbutton);
	layout->addWidget(m_button5);
	layout->addWidget(m_button6);
	layout->addWidget(m_progressBar);
	layout->addWidget(scrollBar);
	setCentralWidget(new QWidget);
	centralWidget()->setLayout(layout);
	this->setWindowTitle("ovedrive kernel downloader");
	InitSlots();

}

void QtWidgetsApplication1::StartDownLoad()
{
	if (!GetIsLogIn())
	{
		QMessageBox::information(NULL, "Attention", "please log in");
		return;
	}
	m_download->DoDownload();
}

void QtWidgetsApplication1::ShowDownLoadResult()
{
	QByteArray context = m_download->GetContext();
	//if (context.size()>0)
	//{
	//	m_line->setText("OK");
	//}
	//else
	//{
	//	m_line->setText("fail to update");
	//}
	m_line->setText(context);
}

void QtWidgetsApplication1::DownLoadResult()
{
	//get the button row
	int row = 0;
	QPushButton* button = qobject_cast<QPushButton*>(this->sender());
	if (button)
	{	
		QModelIndex index = m_resTable->indexAt(QPoint(button->pos().x(), button->pos().y()));
		row = index.row();
	}
	m_progressBar->reset();
	m_progressBar->setVisible(true);
	QTableWidgetItem* item = m_resTable->item(row, 0);
	if (item !=nullptr)
	{
		QString name = item->text();
		QByteArray fileContent;
		try {
			QString savePath;
			savePath = QFileDialog::getExistingDirectory(nullptr, "choose directory", QDir::currentPath());
			if(!savePath.isEmpty())
			{
				QString url = QString(g_DownloadPath+"/Overdrive/").append(name);
				m_download->DownloadResource(url, savePath);
				m_output->setText(m_download->GetResult());
			}
			else
			{
				m_output->setText("The file path error");
			}
		}
		catch (std::exception e)
		{
			m_output->setText(e.what());
		}
	}
}
void QtWidgetsApplication1::UpDateResult()
{
	disconnect(m_download, &Downloader::getdone, this, &QtWidgetsApplication1::UpDateResult);
	m_progressBar->reset();
	m_progressBar->setVisible(true);

	QString latestVersion = QString::fromUtf8(m_download->GetContext());
	QString savePath;
	savePath = QFileDialog::getExistingDirectory(nullptr, "choose directory", QDir::currentPath());
	try {
		if (!savePath.isEmpty())
		{
			QDir dir(savePath);
			if (dir.exists("temp")) 
			{
				QDir tempDir = savePath + "/temp";
				tempDir.removeRecursively();
			}
			else 
			{
				dir.mkdir("temp");
			}
			QString tempPath = savePath + "/temp";
			QString url = QString(g_DownloadPath + "/Overdrive/").append(latestVersion);
			m_download->DownloadResource(url, tempPath);
			m_output->setText(m_download->GetResult());
		}
		else
		{
			m_output->setText("The file path error");
			QMessageBox::critical(nullptr, "error", "the file path error");
			return;
		}
	}
	catch (std::exception e)
	{
		m_output->setText(e.what());
		QDir dir(savePath + "/temp");
		dir.removeRecursively();
	}
	m_download->UpdatePackage(savePath);
}
void QtWidgetsApplication1::ShowResource()
{
	//List 
	if (!GetIsLogIn())
	{
		QMessageBox::information(NULL, "Attention", "please log in");
		return;
	}
	QList<QString> resource = m_download->GetAllResource();
	
	m_resTable->setColumnCount(2);
	m_resTable->setRowCount(resource.size());
	
	m_resTable->setColumnWidth(0, 300);
	for (int i = 0; i < resource.size(); i++)
	{
		QTableWidgetItem* resouceItem = new QTableWidgetItem();
		resouceItem->setText(resource[i]);
		m_resTable->setItem(i, 0, resouceItem);
		
		QPushButton* downloadButton = new QPushButton("Download");
		m_resTable->setCellWidget(i, 1, downloadButton);
		connect(downloadButton, SIGNAL(clicked()), this, SLOT(DownLoadResult()));	
	}
}
void QtWidgetsApplication1::CheckSoftware()
{
	static const std::vector<QString> librarys = {
		"ApiOd.dll",
		"logging.dll",
		"Modeller.dll",
		"pskernel.dll",
		"python3.dll",
		"python37.dll",
		"Render.dll",
		"stlport.5.2.dll",
		"STLportWrapper.dll",
		"ZW3D_Base.dll",
		"ZW3D_Blas.dll",
		"ZW3D_Config.dll",
		"ZW3D_Cons.dll",
		"ZW3D_Core.dll",
		"ZW3D_Db.dll",
		"ZW3D_DimData.dll",
		"ZW3D_Disp.dll",
		"ZW3D_GeomCore.dll",
		"ZW3D_Om.dll",
		"ZW3D_Pin.dll",
		"ZW3D_topo.dll",
		//"xlator/ZW3D_Xlator_common.dll",
		"xlator/ZW3D_Xlator_Iges2Vx.dll",
		"xlator/ZW3D_Xlator_N2Vx.dll",
		"xlator/ZW3D_Xlator_Step2Vx.dll",
		"xlator/ZW3D_Xlator_Vx2Iges.dll",
		"xlator/ZW3D_Xlator_Vx2N.dll",
		"xlator/ZW3D_Xlator_Vx2Step.dll",
		"xlator/ZW3D_Xlator_Vx2Stl.dll"
	};
	if (!GetIsLogIn())
	{
		QMessageBox::information(NULL, "Attention", "please log in");
		return;
	}
	m_download->ClearResult();
	QString folderPath = QFileDialog::getExistingDirectory(0, "choose overdrive directory", QDir::homePath());

	if (folderPath.size() == 0)
	{
		m_output->setText("exmpty folder");
		return;
	}

	for (auto &file:librarys)
	{
		QString fullFile = folderPath+QString("/OverdriveSDK/Kernel/Releasex64/").append(file);
		QFile fullFileName(fullFile);
		
		if (!fullFileName.exists())
		{
			m_output->setText("lack of file:" + file);
			return;
			break;
		}
	}

	m_output->setText("Complete package");
	return;
	//GET PROCESSID
	HANDLE  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		m_output->setText("Failed to create snapshot");
		return;
	}

	PROCESSENTRY32 pe;
	DWORD processId;
	pe.dwSize = sizeof(PROCESSENTRY32);

	int flag = Process32First(hSnapshot, &pe);
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>* converter = new std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>();

	try {
		//find odtesttool.exe
		while (flag != 0)
		{
			clock_t start,end;
			start = clock();
			std::string result = converter->to_bytes(pe.szExeFile);

			if (std::strcmp(result.c_str(), "ODTestTool.exe") == 0)
			{
				processId = pe.th32ProcessID;
			}
			flag = Process32Next(hSnapshot, &pe);
			//if overtime ,throw error
			if ((clock() - start) / CLOCKS_PER_SEC > 5)
			{
				m_output->setText("Couldn't find the process! You should open it!");
				return;
			}
		}
	}
	catch (std::exception e)
	{
		m_output->setText(e.what());
	}

	//MODULE CHECK
	 hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		m_output->setText("Failed to create snapshot");
		return;
	}

	MODULEENTRY32 me;
	me.dwSize = sizeof(me);

	if (!Module32First(hSnapshot, &me)) {
		m_output->setText("Failed to get first module");
		CloseHandle(hSnapshot);
		return;
	}
	std::string result;
	do {
		std::string name = converter->to_bytes(me.szModule);
		result += "Module Name: " + name +"\n";
	} while (Module32Next(hSnapshot, &me));
	m_output->setText(result.c_str());

}
void QtWidgetsApplication1::SendLog()
{
	//m_progressBar->reset();
	//m_progressBar->setVisible(true);
	if (!GetIsLogIn())
	{
		QMessageBox::information(NULL, "Attention", "please log in");
		return;
	}
	QString logPath = QFileDialog::getOpenFileName(this, "open files", QDir::currentPath(), "日志文件(*.txt;*.json)");
	int index = logPath.lastIndexOf(".");
	//m_download->CreateLogFolder(logPath);
	m_download->UploadLog(logPath);
	//m_download->UpdateLog("", "");

	m_output->setText(m_download->GetResult());
}
void QtWidgetsApplication1::InitSlots()
{
	connect(m_loginButton,SIGNAL(clicked()), this, SLOT(showLogin()));
	connect(m_downloadLicenseButton,SIGNAL(clicked()), this, SLOT(DownloadLicense()));
	connect(m_checkbutton, SIGNAL(clicked()), this, SLOT(VerifyLicense()));
	connect(m_button1,SIGNAL(clicked()), this, SLOT(StartDownLoad()));
	connect(m_button2,SIGNAL(clicked()), this, SLOT(ShowDownLoadResult()));
	connect(m_button3,SIGNAL(clicked()), this, SLOT(ShowResource()));
	connect(m_button5,SIGNAL(clicked()), this, SLOT(CheckSoftware()));
	connect(m_button6, SIGNAL(clicked()), this, SLOT(SendLog()));
	connect(m_loginWidget, SIGNAL(loginClicked(int)), this, SLOT(SetLogIn(int)));
	connect(m_download, &Downloader::updateProgress, this, &QtWidgetsApplication1::ShowProgress);
	connect(m_download, &Downloader::getdone, this, &QtWidgetsApplication1::UpDateResult, Qt::UniqueConnection);
}
void QtWidgetsApplication1::ShowProgress(qint64 received,qint64 total,qreal progress)
{
	m_progressBar->setValue(progress);
	QString text = "current progress:" + QString::number(progress) + "%";
	m_progressBar->setFormat(text);
}

void QtWidgetsApplication1::DownloadLicense()
{
	if (m_isLogInlevel >= AuthorizationLevel0 && m_isLogInlevel <= AuthorizationLevel2)
	{
		m_download->DownloadlicensFile(m_isLogInlevel);
	}
	else
	{
		QMessageBox::information(NULL, "Attention", "please log in");
		return;
	}
}

void QtWidgetsApplication1::VerifyLicense()
{
	if (m_isLogInlevel >= AuthorizationLevel0 && m_isLogInlevel <= AuthorizationLevel2)
	{
		m_download->VerifylicenseFile();
	}
	else
	{
		QMessageBox::information(NULL, "Attention", "please log in");
		return;
	}
}

void QtWidgetsApplication1::showLogin()
{
	m_loginWidget->show();
}

void QtWidgetsApplication1::SetLogIn(int level)
{
	if (level>=0&&level<=3)
	{
		m_loginWidget->hide();
	}
	
	m_isLogInlevel = level;
}

bool QtWidgetsApplication1::GetIsLogIn() const
{
	return m_isLogInlevel>=0&& m_isLogInlevel<=3;
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{}
