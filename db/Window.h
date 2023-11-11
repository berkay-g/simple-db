#pragma once

#include <Windows.h>
#include <vector>
#include <tchar.h>
#include <string>

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

class Window {
public:
    void UnusedParam() {};
    Window(HINSTANCE hInstance, const TCHAR* title = _T("My Window"), int nWidth = 500, int nHeight = 500, int minX = 300, int minY = 300, DWORD flags = WS_OVERLAPPEDWINDOW);
    int Run();
    void Close();

    HWND AddButton(const TCHAR* buttonText, int x, int y, int width, int height, void (*onClick)(Window&), DWORD flags = (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON), void (*onRightClick)(Window&) = NULL);

    HWND AddLabel(const TCHAR* labelText, int x, int y, int width, int height, DWORD flags = (WS_VISIBLE | WS_CHILD | WS_BORDER | SS_SUNKEN | SS_CENTER));

    HWND AddTextBox(int x, int y, int width, int height, DWORD flags = (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_WANTRETURN | ES_MULTILINE));
    const TCHAR* GetTextBoxText(HWND hWnd);

    HWND AddListBox(int x, int y, int width, int height, void (*selCallback)(Window&), DWORD flags = (WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | WS_VSCROLL));
    int GetSelectedListBoxItem(HWND listBox);
    void UpdateListBox(HWND listBox, const std::vector<std::wstring>& items);

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
    
    struct ControlInfo
    {
        int id;
        HWND hwnd;
        void (*onClick)(Window&);
        void (*onRightClick)(Window&);

        struct Properties
        {
            int x, y, width, height;
            unsigned long flags;

            const TCHAR* name = NULL;
        } properties;
    };

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND hwnd;
    HINSTANCE hInstance;
    int minX, minY;
    int iWidth, iHeight;

    std::vector<ControlInfo> controls;

    void (*ResizeCallback)(Window& window, HWND hWnd, int newWidth, int newHeight) = NULL;

    static LRESULT CALLBACK SubclassButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

public:
    int GetControlSize() const
    {
        return static_cast<int>(controls.size());
    }
    ControlInfo GetControlInfo(int index) const
    {
        return controls[index];
    }
    int GetInitialWindowHeight() const
    {
        return iHeight;
    }
    int GetInitialWindowWidth() const
    {
        return iWidth;
    }
};
