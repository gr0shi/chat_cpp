#include "MyForm.h"

#include <Windows.h>
using namespace Project4; /* вместо Project2 написать название своего проекта */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);
  Application::Run(gcnew MyForm);
  return 0;
}
