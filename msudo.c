#include <windows.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: msudo <command> [arguments]\n");
    return 1;
  }
    // Create a text buffer to hold all the extra arguments combined
    char parameters[1024] = "";
    
    // Loop through any arguments starting after the main program name
    for (int i = 2; i < argc; i++) {
        strcat_s(parameters, sizeof(parameters), argv[i]);
        if (i < argc - 1) {
            strcat_s(parameters, sizeof(parameters), " "); // Add spaces between words
        }
    }

    SHELLEXECUTEINFO sei = { 0 };
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_DEFAULT;
    sei.lpVerb = "runas";
    sei.lpFile = argv[1];
    sei.lpParameters = parameters; // <--- This sends the extra flags to Windows!
    sei.nShow = SW_NORMAL;

    
    if (!ShellExecuteEx(&sei)) {
        printf("Failed to elevate privileges.\n");
        return 1;
    }

    return 0;


}
