// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/filedlg.h>
#include <wx/gauge.h>
#include <wx/clipbrd.h>
#include <wx/notifmsg.h>
#include <wx/aboutdlg.h>

#include "Downloader.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};
class MyFrame : public wxFrame,public DownloadCallBack
{
public:
    MyFrame(const wxString& title);
private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnResize(wxSizeEvent& event);
    virtual void onMessage(const std::string& str, MESSAGE_TYPE type) override;
    virtual void onProgress(size_t total, size_t current) override;
    virtual void onStart() override;
    virtual void onFinished() override;
    virtual FILE* onSave(std::string filename) override;
    virtual void onURLChanged() override;

    wxDECLARE_EVENT_TABLE();

    wxPanel* mPanel = nullptr;
    wxBoxSizer* mMainlayoutContainer = nullptr;
    wxStaticText* mThemeUrlHint = nullptr;
    wxTextCtrl* mThemeUrl = nullptr;
    wxStaticText* mThemeDownloadUrlHint = nullptr;
    wxTextCtrl* mThemeDownloadUrl = nullptr;
    wxRadioBox* mVersionSelector = nullptr;
    wxButton* openStore = nullptr;
    wxButton* getDownloadLink = nullptr;
    wxButton* copyDownloadLink = nullptr;
    wxButton* downloadThemeAsFile = nullptr;
    wxGauge* mGauge = nullptr;
    Downloader* mDownloader = nullptr;
    FILE* mThemeFile = nullptr;
    bool downloadLinkValid = false;
    std::string mVersion = "V14";
};
enum
{
    ID_RADIOBUTTON_GROUP = 1,
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame("MIUI����������");
    frame->Show(true);
    return true;
}
MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title,wxDefaultPosition)
{
    SetIcon(wxICON(appicon));
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT,"�˳�...\tCtrl+Q");
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT,"����...\tCtrl+H");
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&�ļ�");
    menuBar->Append(menuHelp, "&����");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("��ӭʹ��MIUI����������");
    
    mPanel = new wxPanel(this,wxID_ANY,wxDefaultPosition,FromDIP(wxSize(480,-1)));
    mDownloader = new Downloader(this);
    mMainlayoutContainer = new wxBoxSizer(wxVERTICAL);
    mThemeUrlHint = new wxStaticText(mPanel, wxID_ANY, "MIUI��������");
    wxSize defaultWindowSize = wxDefaultSize;
    wxSize size = FromDIP(wxSize(460, defaultWindowSize.GetHeight()));
    mThemeUrl = new wxTextCtrl(mPanel, wxID_ANY, "", wxDefaultPosition, size, wxTE_PROCESS_ENTER);
    mThemeUrl->Bind(wxEVT_TEXT_ENTER, [&](wxCommandEvent& event) {
        downloadLinkValid = false;
        mDownloader->tryget(mThemeUrl->GetLineText(0).ToStdString(), GET_DOWNLOAD_LINK_URL,mVersion);
        });
    mThemeDownloadUrlHint = new wxStaticText(mPanel, wxID_ANY, "������������");
    mThemeDownloadUrl = new wxTextCtrl(mPanel, wxID_ANY, "", wxDefaultPosition, size, wxTE_READONLY);

    int borderSize = GetDPIScaleFactor() * 10;
    int verticalSpace = GetDPIScaleFactor() * 7;

    mMainlayoutContainer->Add(mThemeUrlHint, wxSTRETCH_NOT, wxALIGN_LEFT | wxLEFT | wxTOP | wxRIGHT, borderSize);
    mMainlayoutContainer->Add(mThemeUrl, wxEXPAND, wxALIGN_LEFT | wxLEFT | wxRIGHT , borderSize);
    mMainlayoutContainer->AddSpacer(verticalSpace);
    mMainlayoutContainer->Add(mThemeDownloadUrlHint, wxSTRETCH_NOT, wxALIGN_LEFT | wxLEFT | wxRIGHT, borderSize);
    mMainlayoutContainer->Add(mThemeDownloadUrl, wxSTRETCH_NOT, wxALIGN_LEFT | wxLEFT | wxRIGHT, borderSize);
    wxArrayString choice;
    choice.Add("V4");
    choice.Add("V5");
    choice.Add("V6/V7  ");
    choice.Add("V8/V9  ");
    choice.Add("V10");
    choice.Add("V11");
    choice.Add("V12");
    choice.Add("V13");
    choice.Add("V14");
    mVersionSelector = new wxRadioBox(mPanel, ID_RADIOBUTTON_GROUP, "", wxDefaultPosition, size, choice);
    mVersionSelector->SetSelection(choice.size()-1);
    mVersionSelector->Bind(wxEVT_RADIOBOX, [&](wxCommandEvent& event) {
        std::string version = mVersionSelector->GetString(mVersionSelector->GetSelection()).ToStdString();
        if (version == "V6/V7  ") 
        {
            mVersion = "V6";
        }
        else if (version == "V8/V9  ") 
        {
            mVersion = "V8";
        }
        else
        {
            mVersion = version;
        }
        });
    mMainlayoutContainer->Add(mVersionSelector, wxSTRETCH_NOT, wxALIGN_LEFT | wxLEFT | wxRIGHT, borderSize);

    wxBoxSizer* buttonsContainer = new wxBoxSizer(wxHORIZONTAL);
    int horizonSpace = GetDPIScaleFactor() * 2;
    wxSize buttonSize = FromDIP(wxSize((480 - 20 - 2 * 3) / 4,defaultWindowSize.GetHeight()));
    openStore = new wxButton(mPanel, wxID_ANY, "�������̵�",wxDefaultPosition, buttonSize);
    getDownloadLink = new wxButton(mPanel, wxID_ANY, "��ȡ��������", wxDefaultPosition, buttonSize);
    copyDownloadLink = new wxButton(mPanel, wxID_ANY, "������������", wxDefaultPosition, buttonSize);
    downloadThemeAsFile = new wxButton(mPanel, wxID_ANY, "����", wxDefaultPosition, buttonSize);
    openStore->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
        ShellExecute(NULL, L"open", L"http://zhuti.xiaomi.com/", NULL, NULL, SW_SHOWNORMAL);
        });

    getDownloadLink->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
        mDownloader->tryget(mThemeUrl->GetLineText(0).ToStdString(), GET_DOWNLOAD_LINK_URL, mVersion);
        });

    copyDownloadLink->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
        if (!downloadLinkValid)
        {
            mDownloader->tryget(mThemeUrl->GetLineText(0).ToStdString(), GET_DOWNLOAD_LINK_URL, mVersion);
            if (!downloadLinkValid) {
                return;
            }
        }
        wxClipboard wxclipboard;
        if (wxclipboard.Open())
        {
            wxclipboard.SetData(new wxTextDataObject(mThemeDownloadUrl->GetLineText(0)));
            wxclipboard.Close();
            wxNotificationMessage notification("�����ɹ�", mThemeDownloadUrl->GetLineText(0),mPanel);
            // auto hide
            notification.Show(-1);
        }
        });

    downloadThemeAsFile->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
        if (!downloadLinkValid)
        {
            mDownloader->tryget(mThemeUrl->GetLineText(0).ToStdString(), GET_DOWNLOAD_LINK_URL, mVersion);
            if (!downloadLinkValid) {
                return;
            }
        }
        mDownloader->tryget(mThemeDownloadUrl->GetLineText(0).ToStdString(), DOWNLOAD_THEME_AS_FILE, mVersion);
        });

    buttonsContainer->Add(openStore, wxSTRETCH_NOT);
    buttonsContainer->Add(getDownloadLink, wxSTRETCH_NOT, wxLEFT, horizonSpace);
    buttonsContainer->Add(copyDownloadLink, wxSTRETCH_NOT, wxLEFT, horizonSpace);
    buttonsContainer->Add(downloadThemeAsFile, wxSTRETCH_NOT,wxLEFT, horizonSpace);
    mMainlayoutContainer->AddSpacer(verticalSpace);
    mMainlayoutContainer->Add(buttonsContainer, wxSTRETCH_NOT,  wxLEFT | wxRIGHT, borderSize);

    mGauge = new wxGauge(mPanel, wxID_ANY, 100, wxDefaultPosition, size, wxGA_HORIZONTAL);
    mMainlayoutContainer->AddSpacer(verticalSpace);
    mMainlayoutContainer->Add(mGauge, wxSTRETCH_NOT, wxLEFT | wxBOTTOM | wxRIGHT, borderSize);
    mMainlayoutContainer->SetMinSize(FromDIP(wxSize(480, -1)));

    mPanel->SetSizerAndFit(mMainlayoutContainer);

    // forbid resize window size
    SetWindowStyle(GetWindowStyle() ^ wxMAXIMIZE_BOX);
    Fit();
    wxSize windowsize = mPanel->GetClientSize();
    SetMinClientSize(windowsize);
    SetMaxClientSize(windowsize);
    Center();
}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("���ڱ�����");
    aboutInfo.SetDescription(_("С����������������"));
    aboutInfo.SetCopyright("(C) 2018-2022");
    aboutInfo.SetWebSite("https://github.com/BDZNH/");
    aboutInfo.AddDeveloper("BDZNH");
    wxAboutBox(aboutInfo);
}

void MyFrame::onMessage(const std::string& str, MESSAGE_TYPE type)
{
    switch (type)
    {
    case GET_LINK_SUCCESS:
        mThemeDownloadUrl->SetLabel(str);
        downloadLinkValid = true;
        break;
    case GET_LINK_FAILED:
        wxMessageBox(str, "����", wxOK | wxICON_WARNING);
        break;
    case DOWNLAOD_SUCCESS:
        wxMessageBox("���سɹ�", "����", wxOK | wxICON_INFORMATION);
        break;
    case DOWNLOAD_FAILED:
        {wxMessageBox(str, "����", wxOK | wxICON_WARNING); }
        break;
    case INVALID_MIUI_THEME_URL:
    {
        std::string msg;
        if (str == "") {
            msg = "��������������";
        }
        else 
        {
            msg = "\"" + str + "\"������Ч����������";
        }
        wxMessageBox(msg, "����", wxOK | wxICON_WARNING);
    }
        break;
    default:
        break;
    }
    
}
void MyFrame::onProgress(size_t total, size_t current)
{
    if (total != 0) {
        // Ӧ�ò�������ɣ��Ͼ�ֻ��һ�������
        mGauge->SetValue((current >= total) ? 100 : (current * 100 / total));
    }
}
void MyFrame::onStart()
{
    openStore->Disable();
    getDownloadLink->Disable();
    copyDownloadLink->Disable();
    downloadThemeAsFile->Disable();
}
void MyFrame::onFinished()
{
    if (mThemeFile)
    {
        fclose(mThemeFile);
        mThemeFile = nullptr;
    }
    openStore->Enable();
    getDownloadLink->Enable();
    copyDownloadLink->Enable();
    downloadThemeAsFile->Enable();
    mGauge->SetValue(0);
}

FILE* MyFrame::onSave(std::string filename)
{
    std::string filenameWithoutExtract = filename.substr(0, filename.find_first_of('.'));
    wxFileDialog saveFileDialog(this, filename, "./", filenameWithoutExtract, "(*.mtz)|*.mtz", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return nullptr;
    }
    mThemeFile = fopen(saveFileDialog.GetPath(),"wb+");
    if (!mThemeFile)
    {
        wxMessageBox("��Ч�ı���λ��", "����", wxOK | wxICON_WARNING);
        return nullptr;
    }
    return mThemeFile;
}

void MyFrame::onURLChanged()
{
    mThemeDownloadUrl->Clear();
    downloadLinkValid = false;
}

