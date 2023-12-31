#include "Window.h"
#include "resource.h"


Window::Window(HINSTANCE hInstance, const TCHAR* title, int nWidth, int nHeight, int minX, int minY, DWORD flags) : hInstance(hInstance)
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

    this->minX = minX;
    this->minY = minY;

    this->iWidth = nWidth;
    this->iHeight = nHeight;
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

void Window::Close()
{
    PostQuitMessage(0);
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
                return 0;

            break;
        }
        case WM_CTLCOLORSTATIC:
            SetTextColor((HDC)wParam, RGB(109, 194, 222));
            SetBkMode((HDC)wParam, TRANSPARENT);
            return GetClassLongPtr(hwnd, GCLP_HBRBACKGROUND);
        case WM_SIZE:
            if (pThis->ResizeCallback != NULL)
            {
                pThis->ResizeCallback(*pThis, hwnd, LOWORD(lParam), HIWORD(lParam));
            }
            return 0;
        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = pThis->minX;
            lpMMI->ptMinTrackSize.y = pThis->minY;
            return 0;
        }
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

LRESULT Window::SubclassButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == WM_RBUTTONDOWN)
    {
        void (*onRightClick)() = reinterpret_cast<void (*)()>(dwRefData);
        if (onRightClick)
        {
            onRightClick();
        }
        return 0; // Prevent the default button behavior
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}


HWND Window::AddButton(const TCHAR* buttonText, int x, int y, int width, int height, void (*onClick)(Window&), DWORD flags, void (*onRightClick)(Window&))
{
    int id = static_cast<int>(controls.size()) + 1; // Generate a unique ID for the control
    HWND button = CreateWindow(_T("BUTTON"), buttonText, flags, x, y, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
    controls.push_back({ id, button, onClick, onRightClick, x, y, width, height, flags, buttonText });

    SetWindowSubclass(button, SubclassButtonProc, id, (DWORD_PTR)onRightClick);

    return button;
}

HWND Window::AddLabel(const TCHAR* labelText, int x, int y, int width, int height, DWORD flags)
{
    int id = static_cast<int>(controls.size()) + 1; // Generate a unique ID for the control
    HWND label = CreateWindow(_T("STATIC"), labelText, flags, x, y, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
    controls.push_back({ id, label, nullptr, nullptr, { x, y, width, height, flags, labelText } });
    return label;
}

HWND Window::AddTextBox(int x, int y, int width, int height, DWORD flags)
{
    int id = static_cast<int>(controls.size()) + 1; // Generate a unique ID for the control
    HWND textbox = CreateWindow(_T("EDIT"), _T(""), flags, x, y, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
    controls.push_back({ id, textbox, nullptr, nullptr, { x, y, width, height, flags, L"" } });
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
    controls.push_back({ id, listbox, selCallback, nullptr, { x, y, width, height, flags, L"" } });
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
            controls.push_back({ id, radioButton, onClick, nullptr, { xPosition, yPosition, width, height, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, labels[i].data()} });
        }
        else
        {
            radioButton = CreateWindow(_T("BUTTON"), labels[i].data(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                xPosition, yPosition, width, height, hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInstance, NULL);
            controls.push_back({ id, radioButton, onClick, nullptr, { xPosition, yPosition, width, height, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, labels[i].data()}});
        }

        int initialCheckState = (i == selectedButton) ? BST_CHECKED : BST_UNCHECKED;
        SendMessage(radioButton, BM_SETCHECK, initialCheckState, 0);


        hWnds.push_back(radioButton);
        horizontal ? xPosition += width + 10 : yPosition += height + 10;
    }
    return hWnds;

    /*
    std::vector<HWND> radios;
    void func(Window& window)
    {

        int checkState = (int)SendMessage(radios[1], BM_GETCHECK, 0, 0);

        if (checkState == BST_CHECKED) {
            MessageBox(NULL, _T("Radio button is checked!"), _T("Radio Button Checked"), MB_ICONINFORMATION | MB_OK);
        } else {
            // The radio button is not checked
            // Handle the unchecked state here
        }
    }
    */
}

void Window::AddMenu(HMENU parentMenu, const TCHAR* menuName, int subMenuCount, ...)
{
    HMENU hSubMenu = CreateMenu();
    va_list args;
    va_start(args, subMenuCount);
    for (int i = 0; i < subMenuCount; i++)
    {
        const TCHAR* itemName = va_arg(args, const TCHAR*);
        if (itemName == NULL)
            break;

        int id = static_cast<int>(controls.size()) + 1;

        typedef void (*FunctionPtr)(Window&);
        FunctionPtr func = va_arg(args, FunctionPtr);

        controls.push_back({ id, NULL, func, nullptr });

        AppendMenu(hSubMenu, MF_STRING, id, itemName);
    }
    va_end(args);
    AppendMenu(parentMenu, MF_POPUP, (UINT_PTR)hSubMenu, menuName);
}

void Window::ApplyMenu(HMENU hMenu)
{
    SetMenu(hwnd, hMenu);
}

void Window::ModifyControl(HWND control, const TCHAR* newText, int x, int y, int width, int height, DWORD newStyle, bool isButton)
{
    SetWindowText(control, newText);  // Change the control's text
    SetWindowPos(control, NULL, x, y, width, height, SWP_NOZORDER);  // Change position and size
    SetWindowLongPtr(control, GWL_STYLE, newStyle);  // Change control style (flags)

    // Redraw the control based on its type
    if (isButton) {
        // For buttons
        InvalidateRect(control, NULL, TRUE);
    }
    else {
        // For labels or other controls
        RedrawWindow(control, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE);
    }
}

void Window::ChangeFont(HWND hWnd, const TCHAR* fontName, int fontSize, int fontWeight, bool isItalic, bool isUnderline, bool isStrikeOut)
{
    HFONT newFont = CreateFont(fontSize, 0, 0, 0, fontWeight, isItalic, isUnderline, isStrikeOut, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, fontName);

    SendMessage(hWnd, WM_SETFONT, (WPARAM)newFont, TRUE);
}

void Window::SetResizeCallback(void(*Callback)(Window& window, HWND hWnd, int newWidth, int newHeight))
{
    ResizeCallback = Callback;

    /*
    void WindowResizeCallback(Window& window, HWND hWnd, int newWidth, int newHeight) {

        struct Button {
            int x, y, w, h;
        };
    
        float widthScale = static_cast<float>(newWidth) / (window.GetInitialWindowWidth() - 16);
        float heightScale = static_cast<float>(newHeight) / (window.GetInitialWindowHeight() - 39);
    
        double scaleFactor = sqrt(widthScale * heightScale);
        int minFontSize = 15;
    
        for (int i = 0; i < window.GetControlSize(); i++)
        {
            Button button = { window.GetControlInfo(i).properties.x, window.GetControlInfo(i).properties.y, window.GetControlInfo(i).properties.width, window.GetControlInfo(i).properties.height };
            // Resize the button proportionately
            button.w = static_cast<int>(button.w * widthScale);
            button.h = static_cast<int>(button.h * heightScale);
    
            // Reposition the button proportionately
            button.x = static_cast<int>(button.x * widthScale);
            button.y = static_cast<int>(button.y * heightScale);
    
            window.ChangeFont(window.GetControlInfo(i).hwnd, L"Arial", static_cast<int>(max(16 * scaleFactor, minFontSize)), FW_DEMIBOLD);
            if (window.GetControlInfo(i).properties.name != NULL)
            {
                window.ModifyControl(window.GetControlInfo(i).hwnd, window.GetTextBoxText(window.GetControlInfo(i).hwnd), button.x, button.y, button.w, button.h, window.GetControlInfo(i).properties.flags, false);
            }
        }
    }
    */
}
