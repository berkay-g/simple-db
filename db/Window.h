#pragma once

#include <Windows.h>
#include <vector>
#include <tchar.h>
#include <string>

class Window {
public:
    void UnusedParam() {};
    Window(HINSTANCE hInstance, const TCHAR* title = _T("My Window"), int nWidth = 500, int nHeight = 500, DWORD flags = WS_OVERLAPPEDWINDOW);
    int Run();
    void Close();

    HWND AddButton(const TCHAR* buttonText, int x, int y, int width, int height, void (*onClick)(Window&), DWORD flags = (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON));

    HWND AddLabel(const TCHAR* labelText, int x, int y, int width, int height, DWORD flags = (WS_VISIBLE | WS_CHILD | WS_BORDER | SS_SUNKEN | SS_CENTER));

    HWND AddTextBox(int x, int y, int width, int height, DWORD flags = (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_WANTRETURN | ES_MULTILINE));
    const TCHAR* GetTextBoxText(HWND hWnd);

    HWND AddListBox(int x, int y, int width, int height, void (*selCallback)(Window&), DWORD flags = (WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | WS_VSCROLL));
    int GetSelectedListBoxItem(HWND listBox);
    void UpdateListBox(HWND listBox, const std::vector<std::wstring>& items);

    std::vector<HWND> AddRadioButtons(int numRadioButtons, const TCHAR* labels[], int x, int y, int width, int height, int selectedButton, void (*onClick)(Window&), bool horizontal = true);
    std::vector<HWND> AddRadioButtons(int numRadioButtons, const std::vector<std::wstring>& labels, int x, int y, int width, int height, int selectedButton, void (*onClick)(Window&), bool horizontal = true);

    // Example Usage: // Function prototype -> void foo(Window&)
    // HMENU hMenu = CreateMenu();
    // AddMenu(hMenu, _T("File"), 3, _T("New"), newFunction, _T("Open"), openFunction, _T("Exit"), exitFunction); 
    // AddMenu(hMenu, _T("Edit"), 3, _T("Copy"), copyFunction, _T("If No Function"), NULL, _T("Paste"), pasteFunction);
    // ApplyMenu(hMenu);
    void AddMenu(HMENU parentMenu, const TCHAR* menuName, int subMenuCount, ...);
    void ApplyMenu(HMENU hMenu);

    void ModifyControl(HWND control, const TCHAR* newText, int x, int y, int width, int height, DWORD newStyle, bool isButton);
    void ChangeFont(HWND hWnd, const TCHAR* fontName, int fontSize, int fontWeight = FW_NORMAL, bool isItalic = false, bool isUnderline = false, bool isStrikeOut = false);

    void SetResizeCallback(void(*Callback)(Window& window, HWND hWnd, int newWidth, int newHeight));
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND hwnd;
    HINSTANCE hInstance;
    int minX, minY;

    struct ControlInfo
    {
        int id;
        HWND hwnd;
        void (*onClick)(Window&);
    };
    std::vector<ControlInfo> controls;

    void (*ResizeCallback)(Window& window, HWND hWnd, int newWidth, int newHeight) = NULL;
};
