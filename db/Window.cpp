#include "Window.h"
#include "resource.h"

Window::Window(HINSTANCE hInstance, const TCHAR* title, int nWidth, int nHeight, DWORD flags) : hInstance(hInstance)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, title, NULL };

    #ifdef IDI_ICON1
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
        wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    #endif // IDI_ICON1

    wc.hbrBackground = CreateSolidBrush(RGB(64, 64, 64));
    RegisterClassEx(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN); // Screen width
    int screenHeight = GetSystemMetrics(SM_CYSCREEN); // Screen height

    int x = (screenWidth - nWidth) / 2; // Calculate the X position
    int y = (screenHeight - nHeight) / 2; // Calculate the Y position

    // Create the window
    hwnd = CreateWindow(wc.lpszClassName, title, flags, x, y, nWidth, nHeight, NULL, NULL, wc.hInstance, this);

    ShowWindow(hwnd, SW_SHOW);
}

int Window::Run() 
{
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    Window* pThis = nullptr;

    if (uMsg == WM_NCCREATE) 
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = static_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) 
    {
        switch (uMsg) 
        {
        case WM_COMMAND: 
        {
            int controlId = LOWORD(wParam);
            bool handled = false;
            for (const auto& control : pThis->controls) {
                if (control.id == controlId) {
                    if (HIWORD(wParam) == BN_CLICKED && control.onClick) {
                        control.onClick(*pThis);
                        handled = true;
                        break;
                    }

                    if (HIWORD(wParam) == LBN_SELCHANGE && control.onClick) {
                        control.onClick(*pThis);
                        handled = true;
                        break;
                    }
                }
            }
            if (handled) 
            {
                return 0;
            }
            break;
        }
        case WM_CTLCOLORSTATIC:
            SetTextColor((HDC)wParam, RGB(109, 194, 222));
            SetBkMode((HDC)wParam, TRANSPARENT);
            return GetClassLongPtr(hwnd, GCLP_HBRBACKGROUND);
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    else 
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


HWND Window::AddButton(const TCHAR* buttonText, int x, int y, int width, int height, void (*onClick)(Window&), DWORD flags)
{
    int id = static_cast<int>(controls.size()) + 1; // Generate a unique ID for the control
    HWND button = CreateWindow(_T("BUTTON"), buttonText, flags, x, y, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
    controls.push_back({ id, button, onClick });
    return button;
}

HWND Window::AddLabel(const TCHAR* labelText, int x, int y, int width, int height, DWORD flags) 
{
    int id = static_cast<int>(controls.size()) + 1; // Generate a unique ID for the control
    HWND label = CreateWindow(_T("STATIC"), labelText, flags, x, y, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
    controls.push_back({ id, label, nullptr });
    return label;
}

HWND Window::AddTextBox(int x, int y, int width, int height, DWORD flags) 
{
    int id = static_cast<int>(controls.size()) + 1; // Generate a unique ID for the control
    HWND textbox = CreateWindow(_T("EDIT"), _T(""), flags, x, y, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
    controls.push_back({ id, textbox, nullptr });
    return textbox;
}

const TCHAR* Window::GetTextBoxText(HWND hWnd)
{
    static TCHAR buffer[512];
    GetWindowText(hWnd, buffer, 512);
    return buffer;
}

HWND Window::AddListBox(int x, int y, int width, int height, void (*selCallback)(Window&), DWORD flags)
{
    int id = static_cast<int>(controls.size()) + 1; // Generate a unique ID for the control
    HWND listbox = CreateWindow(_T("ListBox"), _T(""), flags, x, y, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
    controls.push_back({ id, listbox, selCallback });
    return listbox;
}

int Window::GetSelectedListBoxItem(HWND listBox) 
{
    int index = static_cast<int>(SendMessage(listBox, LB_GETCURSEL, 0, 0));
    if (index != LB_ERR) 
    {
        TCHAR buffer[256]{};
        SendMessage(listBox, LB_GETTEXT, index, (LPARAM)buffer);
        // Now 'buffer' contains the selected item.
        return index;
    }
    return -1; // No item selected or an error occurred.
}

void Window::UpdateListBox(HWND listBox, const std::vector<std::wstring>& items)
{
    SendMessage(listBox, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < items.size(); i++)
    {
        SendMessage(listBox, LB_ADDSTRING, i, (LPARAM)items[i].data());
    }
}

std::vector<HWND> Window::AddRadioButtons(int numRadioButtons, const TCHAR* labels[], int x, int y, int width, int height, int selectedButton, void (*onClick)(Window&), bool horizontal) 
{
    int xPosition = x;
    int yPosition = y;
    std::vector<HWND> hWnds;
    for (int i = 0; i < numRadioButtons; i++) 
    {
        HWND radioButton;
        int id = static_cast<int>(controls.size()) + 1;
        if (i == 0)
        {
            radioButton = CreateWindow(_T("BUTTON"), labels[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
                xPosition, yPosition, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
        }
        else
        {
            radioButton = CreateWindow(_T("BUTTON"), labels[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                xPosition, yPosition, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
        }
        controls.push_back({ id, radioButton, onClick });

        int initialCheckState = (i == selectedButton) ? BST_CHECKED : BST_UNCHECKED;
        SendMessage(radioButton, BM_SETCHECK, initialCheckState, 0);


        hWnds.push_back(radioButton);
        horizontal ? xPosition += width + 10 : yPosition += height + 10; 
    }
    return hWnds;
}

std::vector<HWND> Window::AddRadioButtons(int numRadioButtons, const std::vector<std::wstring>& labels, int x, int y, int width, int height, int selectedButton, void (*onClick)(Window&), bool horizontal)
{
    int xPosition = x;
    int yPosition = y;
    std::vector<HWND> hWnds;
    for (int i = 0; i < numRadioButtons; i++)
    {
        HWND radioButton;
        int id = static_cast<int>(controls.size()) + 1;
        if (i == 0)
        {
            radioButton = CreateWindow(_T("BUTTON"), labels[i].data(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
                xPosition, yPosition, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
        }
        else
        {
            radioButton = CreateWindow(_T("BUTTON"), labels[i].data(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                xPosition, yPosition, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
        }
        controls.push_back({ id, radioButton, onClick });

        int initialCheckState = (i == selectedButton) ? BST_CHECKED : BST_UNCHECKED;
        SendMessage(radioButton, BM_SETCHECK, initialCheckState, 0);


        hWnds.push_back(radioButton);
        horizontal ? xPosition += width + 10 : yPosition += height + 10;
    }
    return hWnds;
}