#include <windows.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: msudo <command> [arguments]\n");
    return 1;
  }

    SHELLEXECUTEINFO sei = { 0 };
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_DEFAULT;
    sei.lpVerb = "runas";
    sei.lpFile = argv[1];
    sei.nShow = SW_NORMAL;

    if (!ShellExecuteEx(&sei)) {
        printf("Failed to elevate privileges.\n");
        return 1;
    }

    return 0;


}
