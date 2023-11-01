#include <codecvt>

#include "Window.h"
#include "db.h"

HWND nameTextBox, idTextBox;
HWND nameListBox, staticId;
Database db("data.db");
std::vector<std::wstring> nameList;
std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> str_converter;
void addFunction(Window& window);
void deleteFunction(Window& window);
void listBoxSelectionCallback(Window& window);

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    Window mainWindow(hInstance, _T("Database"));
  
    mainWindow.AddLabel(_T("Hello World!"), 20, 20, 300, 20);

    mainWindow.AddLabel(_T("Name"), 20, 50, 40, 20);
    mainWindow.AddLabel(_T("ID"), 20, 80, 40, 20);

    nameTextBox = mainWindow.AddTextBox(65, 50, 140, 20, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_WANTRETURN | ES_MULTILINE);
    idTextBox = mainWindow.AddTextBox(65, 80, 140, 20, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER);

    mainWindow.AddButton(_T("Add"), 20, 110, 200, 20, addFunction);

    nameList.clear();
    for (int i = 0; i < db.size(); i++)
    {
        nameList.push_back(str_converter.from_bytes(db.getName(i)));
    }
    nameListBox = mainWindow.AddListBox(20, 150, 120, 70, listBoxSelectionCallback, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | WS_VSCROLL);
    mainWindow.UpdateListBox(nameListBox, nameList);

    staticId = mainWindow.AddTextBox(160, 155, 120, 23, WS_CHILD | WS_VISIBLE | WS_BORDER | SS_SUNKEN | SS_CENTER | ES_READONLY);
    mainWindow.AddButton(_T("Delete"), 160, 190, 120, 23, deleteFunction);

    return mainWindow.Run();
}

void addFunction(Window& window)
{
    std::wstring wName = window.GetTextBoxText(nameTextBox);
    std::wstring wId = window.GetTextBoxText(idTextBox);
    if (wName.empty() || wId.empty())
        return;

    db.add(str_converter.to_bytes(wName), std::atoi(str_converter.to_bytes(wId).data()));
    nameList.clear();
    for (int i = 0; i < db.size(); i++)
    {
        nameList.push_back(str_converter.from_bytes(db.getName(i)));
    }
    window.UpdateListBox(nameListBox, nameList);

    SetWindowText(nameTextBox, 0);
    SetWindowText(idTextBox, 0);
}

void deleteFunction(Window& window)
{
    TCHAR id[256];
    if (GetWindowText(staticId, id, 256) == 0)
        return;
    SetWindowText(staticId, 0);
    
    std::wstring wTmp = id;

    db.remove(std::atoi(str_converter.to_bytes(wTmp).data()));
    nameList.clear();
    for (int i = 0; i < db.size(); i++)
    {
        nameList.push_back(str_converter.from_bytes(db.getName(i)));
    }
    window.UpdateListBox(nameListBox, nameList);
}

void listBoxSelectionCallback(Window& window)
{
    int index = window.GetSelectedListBoxItem(nameListBox);
    std::wstring wTmp = std::to_wstring(db.getID(index));

    if (index != -1)
        SendMessage(staticId, WM_SETTEXT, index, (LPARAM)wTmp.data());
}