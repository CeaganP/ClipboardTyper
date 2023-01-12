#include <string>

#include "Source.h"

using namespace TekkitTyper;

/**************** METHODS*/
int main();
bool PrintClipboard();
LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);
/****************/

/**************** VARIABLES*/
bool isExecuting = false; //ensure the printer only executes once
bool startExecuting = false;
bool doCancelPrinting = false; //when pressing the activation key, press again to cancel
/****************/

//https://stackoverflow.com/questions/22975916/global-keyboard-hook-with-wh-keyboard-ll-and-keybd-event-windows/28073683
int main()
{
    std::cout << "Written by Ceagan Paladino" << std::endl;
    std::cout << "To begin, copy text to your clipboard, select target to repeat key strokes in then press [tilde '`~'] key " << std::endl;
    std::cout << std::endl;
    std::cout << "The text you intend to copy must not contain the [tilde '`~'] key otherwise it may intermittently stop copying." << std::endl;

    // Install the low-level keyboard & mouse hooks
    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

    // Keep this app running until we're told to stop
    MSG msg;
    while (!GetMessage(&msg, NULL, NULL, NULL)) {    //this while loop keeps the hook
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hhkLowLevelKybd);

    return(0);
}

HANDLE GetClipboard()
{
    HANDLE h;
    //if the clipboard can open
    if (OpenClipboard(NULL))
    {
        h = GetClipboardData(CF_TEXT);

        printf("%s\n", (char*)h);

        CloseClipboard();
        return h;
    }
    return NULL;
};

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    BOOL fEatKeystroke = FALSE;
    PKBDLLHOOKSTRUCT p = PKBDLLHOOKSTRUCT();

    if (nCode == HC_ACTION)
    {
        p = (PKBDLLHOOKSTRUCT)lParam;
        int keyCode = p->vkCode;
        switch (wParam)
        {
        //case WM_KEYDOWN:
        //case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            
            //http://www.kbdedit.com/manual/low_level_vk_list.html
            //0xC0 = `~
            fEatKeystroke = keyCode == 0xC0;
            //std::cout << "key received   [" << keyCode << "]" << std::endl;

            //DOES NOT FUNCTION AS INTENDED IF THE PRINTED STRING CONTAINS A ` or ~
            //if the vkcode of the current key is `
            /*if (isExecuting && !fEatKeystroke) {
                std::cout << "EVENT CANCELLED";
                doCancelPrinting = true;
            }
            else*/ if (fEatKeystroke)  
            {              
                /*Print the Clipboard on keyup
                if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) // Keydown
                {
                    printClipboard();
                    keybd_event(NULL, 0, 0, 0);
                }
                else */
                //if it isn't printing already then execute
                if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) // Keyup
                {
                    std::cout << "key received" << std::endl;
                    //std::thread task_td(std::move(task));
                    //execute the task as a thread
                    //task_td.join();
                    std::packaged_task<bool()> printTask(PrintClipboard);
                    printTask();
                    std::cout << printTask.get_future().get();
                }
                break;
            }

            //prevKey = p;
            break;
        }
    }

    //if it ate the keystroke then don't call the next hook
    return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}

//https://cboard.cprogramming.com/windows-programming/103976-get-clipboard-text-string.html
bool PrintClipboard()
{
    //if it is already executing cancel execution
    if (!isExecuting)
        isExecuting = true;
    else {
        doCancelPrinting = true;
        return false; //cancel this execution
    }
    std::cout << isExecuting << " " << doCancelPrinting << std::endl;
    HANDLE clip = GetClipboard();
    if (clip == NULL)
        return false;

    HANDLE assert = new HANDLE();
    if (clip != assert)
    {
        unsigned int i = 0;

        while (((char*)clip)[i] != 0 && !doCancelPrinting)
        {
            //https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

            //HWND WindowToFind = FindWindowW(NULL, (LPCWSTR)"Tekkit Classic");
            //SendMessage(WindowToFind, WM_KEYDOWN, key, 0);
            //PostMessage(WindowToFind, WM_KEYDOWN, key, 0);

            char c = ((char*)clip)[i];
            //http://www.kbdedit.com/manual/low_level_vk_list.html
            switch (c)
            {
            case '.':
                keybd_event(VK_OEM_PERIOD, 0, 0, 0);
                keybd_event(VK_OEM_PERIOD, 0, KEYEVENTF_KEYUP, 0);
                break;
            case ',':
                keybd_event(VK_OEM_COMMA, 0, 0, 0);
                keybd_event(VK_OEM_COMMA, 0, KEYEVENTF_KEYUP, 0);
                break;
                /*case '\n':
                    keybd_event(VK_RETURN, 0, 0, 0);
                    keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
                    break;*/
            case '=':
                keybd_event(VK_OEM_PLUS, 0, 0, 0);
                keybd_event(VK_OEM_PLUS, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '+':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(VK_OEM_PLUS, 0, 0, 0);
                keybd_event(VK_OEM_PLUS, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '-':
                keybd_event(VK_OEM_MINUS, 0, 0, 0);
                keybd_event(VK_OEM_MINUS, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '"':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(VK_OEM_7, 0, 0, 0);
                keybd_event(VK_OEM_7, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '(':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(57, 0, 0, 0);
                keybd_event(57, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case ')':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(48, 0, 0, 0);
                keybd_event(48, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '>':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(VK_OEM_PERIOD, 0, 0, 0);
                keybd_event(VK_OEM_PERIOD, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '<':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(VK_OEM_COMMA, 0, 0, 0);
                keybd_event(VK_OEM_COMMA, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '{':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(VK_OEM_4, 0, 0, 0);
                keybd_event(VK_OEM_4, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '}':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(VK_OEM_6, 0, 0, 0);
                keybd_event(VK_OEM_6, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '[':
                keybd_event(VK_OEM_4, 0, 0, 0);
                keybd_event(VK_OEM_4, 0, KEYEVENTF_KEYUP, 0);
                break;
            case ']':
                keybd_event(VK_OEM_6, 0, 0, 0);
                keybd_event(VK_OEM_6, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '/':
                keybd_event(VK_OEM_2, 0, 0, 0);
                keybd_event(VK_OEM_2, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '\\':
                keybd_event(VK_OEM_5, 0, 0, 0);
                keybd_event(VK_OEM_5, 0, KEYEVENTF_KEYUP, 0);
                break;
            case ':':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(VK_OEM_1, 0, 0, 0);
                keybd_event(VK_OEM_1, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case ';':
                keybd_event(VK_OEM_1, 0, 0, 0);
                keybd_event(VK_OEM_1, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '~':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(VK_OEM_3, 0, 0, 0);
                keybd_event(VK_OEM_3, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '`':
                keybd_event(VK_OEM_3, 0, 0, 0);
                keybd_event(VK_OEM_3, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '%':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(53, 0, 0, 0);
                keybd_event(53, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            case '*':
                keybd_event(VK_SHIFT, 0, 0, 0);
                keybd_event(56, 0, 0, 0);
                keybd_event(56, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                break;
            default:
                if (toupper(c) != 0 && isalpha(c))
                {
                    if (toupper(c) == ((char*)clip)[i])
                        keybd_event(VK_SHIFT, 0, 0, 0);
                    
                    keybd_event(toupper(c), 0, 0, 0);    //key down
                    keybd_event(toupper(c), 0, KEYEVENTF_KEYUP, 0); //key up

                    if (toupper(c) == ((char*)clip)[i])
                        keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                }
                else
                {
                    keybd_event(((char*)clip)[i], 0, 0, 0);    //key down
                    keybd_event(((char*)clip)[i], 0, KEYEVENTF_KEYUP, 0); //key up
                }
                break;
            }
            //sleep for a short period so the computer can process
            Sleep(10);
            i++;
            //*pntchr++;
        }
    }
    delete assert;

    isExecuting = false;
    doCancelPrinting = false;

    return true;
}

