#include "QtWidgetsApplication1.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <shellapi.h>

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
	m_button4 = new QPushButton("Download selected installation package");
	

	//layout->addWidget(m_button1);
	//layout->addWidget(m_button2);
	layout->addWidget(m_button3);
	//layout->addWidget(m_line);
	layout->addWidget(m_resTable);
	layout->addWidget(m_button4);

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
	QTableWidgetItem* item = m_resTable->currentItem();
	if (item !=nullptr)
	{
		QString name = item->text();
		QString url = QString::fromUtf8("http://192.168.8.222:8080/Overdrive/").append(name);
		m_download->DownloadResource(url);
	}
}
void QtWidgetsApplication1::ShowResource()
{
	QList<QString> resource = m_download->GetAllResource();
	
	m_resTable->setColumnCount(resource.size());
	QTableWidgetItem* item = new QTableWidgetItem();
	
	for (int i =0;i< resource.size();i++)
	{
		item->setText(resource[0]);
		m_resTable->setItem(i, 0, item);
	}
}

void QtWidgetsApplication1::CheckSoftware()
{
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.lpVerb = LPCWSTR("open");
	sei.lpFile = LPCWSTR("odtesttool.exe");
	sei.hwnd = NULL;
	sei.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&sei);

	DWORD processId = GetProcessId(sei.hProcess);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		std::cerr << "Failed to create snapshot" << std::endl;
		return;
	}

	MODULEENTRY32 me;
	me.dwSize = sizeof(me);

	if (!Module32First(hSnapshot, &me)) {
		std::cerr << "Failed to get first module" << std::endl;
		CloseHandle(hSnapshot);
		return;
	}

	do {
		std::cout << "Module Name: " << me.szModule << std::endl;
	} while (Module32Next(hSnapshot, &me));

	CloseHandle(hSnapshot);
}

void QtWidgetsApplication1::InitSlots()
{
	connect(m_button1,SIGNAL(clicked()), this, SLOT(StartDownLoad()));
	connect(m_button2,SIGNAL(clicked()), this, SLOT(ShowDownLoadResult()));
	connect(m_button3,SIGNAL(clicked()), this, SLOT(ShowResource()));
	connect(m_button4,SIGNAL(clicked()), this, SLOT(DownLoadResult()));
}
QtWidgetsApplication1::~QtWidgetsApplication1()
{}
