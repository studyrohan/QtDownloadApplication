#include "QtWidgetsApplication1.h"
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <locale>
#include <winsock2.h>
#include <shellapi.h>
#include <QFileDialog>
#include "downloader.h"

QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
    : QMainWindow(parent)
{
	m_download = new Downloader(this);
	QVBoxLayout* layout = new QVBoxLayout;

	m_button1 = new QPushButton("update installation package");
	m_button2 = new QPushButton("get result");
	m_button3 = new QPushButton("show resources");
	m_line = new QTextEdit("to update");
	m_line->setReadOnly(true);
	m_resTable = new QTableWidget();
	m_resTable->setRowCount(1);
	m_resTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_button5 = new QPushButton("check overdrive package validity");
	m_button6 = new QPushButton("upload logs");
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
	
	//layout->addWidget(m_button1);
	//layout->addWidget(m_button2);
	layout->addWidget(m_button3);
	//layout->addWidget(m_line);
	layout->addWidget(m_resTable);
	layout->addWidget(m_button5);
	layout->addWidget(m_button6);
	layout->addWidget(scrollBar);
	setCentralWidget(new QWidget);
	centralWidget()->setLayout(layout);

	InitSlots();
}

void QtWidgetsApplication1::StartDownLoad()
{
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
				QString url = QString::fromUtf8("http://192.168.8.222:8080/Overdrive/").append(name);
				m_download->DownloadResource(url, savePath);
				m_output->setText(m_download->GetResult());
			}
			else
			{
				m_output->setText("The file path error！");
			}
		}
		catch (std::exception e)
		{
			m_output->setText(e.what());
		}
	}
}
void QtWidgetsApplication1::ShowResource()
{
	//List 
	QList<QString> resource = m_download->GetAllResource();
	
	m_resTable->setColumnCount(2);
	m_resTable->setRowCount(resource.size());
	
	m_resTable->setColumnWidth(0, 300);
	for (int i =0;i< resource.size();i++)
	{
		QTableWidgetItem* resouceItem = new QTableWidgetItem();
		resouceItem->setText(resource[i]);
		m_resTable->setItem(i, 0, resouceItem);
		
		QPushButton* downloadButton = new QPushButton("Download");
		m_resTable->setCellWidget(i, 1, downloadButton);
		connect(downloadButton, SIGNAL(clicked()), this, SLOT(DownLoadResult()));	
	}
}
void  QtWidgetsApplication1::CheckSoftware()
{
	m_download->ClearResult();
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
	QString logPath = QFileDialog::getOpenFileName(this, "open files", QDir::currentPath(), "日志文件(*.txt;*.js)");
	int index = logPath.lastIndexOf(".");
	//m_download->CreateLogFolder(logPath);
	m_download->UploadLog(logPath);
	//m_download->UpdateLog("", "");

	m_output->setText(m_download->GetResult());
}
void QtWidgetsApplication1::InitSlots()
{
	connect(m_button1,SIGNAL(clicked()), this, SLOT(StartDownLoad()));
	connect(m_button2,SIGNAL(clicked()), this, SLOT(ShowDownLoadResult()));
	connect(m_button3,SIGNAL(clicked()), this, SLOT(ShowResource()));
	connect(m_button5,SIGNAL(clicked()), this, SLOT(CheckSoftware()));
	connect(m_button6, SIGNAL(clicked()), this, SLOT(SendLog()));
}
QtWidgetsApplication1::~QtWidgetsApplication1()
{}
