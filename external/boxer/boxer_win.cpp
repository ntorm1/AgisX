#include <boxer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace boxer {

namespace {

UINT getIcon(Style style) {
   switch (style) {
      case Style::Info:
         return MB_ICONINFORMATION;
      case Style::Warning:
         return MB_ICONWARNING;
      case Style::Error:
         return MB_ICONERROR;
      case Style::Question:
         return MB_ICONQUESTION;
      default:
         return MB_ICONINFORMATION;
   }
}

UINT getButtons(Buttons buttons) {
   switch (buttons) {
      case Buttons::OK:
      case Buttons::Quit: // There is no 'Quit' button on Windows :(
         return MB_OK;
      case Buttons::OKCancel:
         return MB_OKCANCEL;
      case Buttons::YesNo:
         return MB_YESNO;
      default:
         return MB_OK;
   }
}

Selection getSelection(int response, Buttons buttons) {
   switch (response) {
      case IDOK:
         return buttons == Buttons::Quit ? Selection::Quit : Selection::OK;
      case IDCANCEL:
         return Selection::Cancel;
      case IDYES:
         return Selection::Yes;
      case IDNO:
         return Selection::No;
      default:
         return Selection::None;
   }
}

} // namespace

Selection show(const char *message, const char *title, Style style, Buttons buttons) {
   UINT flags = MB_TASKMODAL;

   flags |= getIcon(style);
   flags |= getButtons(buttons);

   wchar_t* wmsg = nullptr;
   wchar_t* wtitle = nullptr;
   if (message) {
       int msglen = static_cast<int>(strlen(message));
       int wmsglen = MultiByteToWideChar(CP_UTF8, 0, message, msglen, nullptr, 0);
       wmsg = new wchar_t[wmsglen + size_t(1)];
       MultiByteToWideChar(CP_UTF8, 0, message, msglen, wmsg, wmsglen);
       wmsg[wmsglen] = L'\0';
   }
   if (title) {
       int titlelen = static_cast<int>(strlen(title));
       int wtitlelen = MultiByteToWideChar(CP_UTF8, 0, title, titlelen, nullptr, 0);
       wtitle = new wchar_t[wtitlelen + size_t(1)];
       MultiByteToWideChar(CP_UTF8, 0, title, titlelen, wtitle, wtitlelen);
       wtitle[wtitlelen] = L'\0';
   }

   auto sel = getSelection(MessageBoxW(nullptr, wmsg, wtitle, flags), buttons);
   if (wmsg) delete[] wmsg;
   if (wtitle) delete[] wtitle;
   return sel;
}

} // namespace boxer
