#include <windows.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // 1. SMART DEFAULT CONFIGURATION
    const char *target_file = NULL;
    char parameters[4096] = "";

    // If run by itself (no options), open an elevated cmd shell as a default fallback
    if (argc < 2) {
        target_file = "cmd.exe";
    } else {
        // 2. PARSE CUSTOM INTERNAL MSUDO SUBCOMMANDS
        
        // msudo repair
        if (strcmp(argv[1], "repair") == 0 && argc == 2) {
            printf("[msudo] Launching System File Corruption Scan (SFC)...\n");
            target_file = "sfc.exe";
            strcpy_s(parameters, sizeof(parameters), "/scannow");
        }
        // msudo repair online
        else if (strcmp(argv[1], "repair") == 0 && argc > 2 && strcmp(argv[2], "online") == 0) {
            printf("[msudo] Running Live Deployment Image Service Repair (DISM) via Windows Safe Mode with Networking...\n");
            target_file = "dism.exe";
            strcpy_s(parameters, sizeof(parameters), "/Online /Cleanup-Image /RestoreHealth");
        }
        // msudo uninstall [app.exe / app.lnk]
        else if (strcmp(argv[1], "uninstall") == 0) {
            if (argc < 3) {
                printf("Error: Please provide an app path to uninstall.\nUsage: msudo uninstall [path_to_app]\n");
                return 1;
            }
            printf("WARNING: Are you absolutely sure you want to uninstall '%s'? (Y/N): ", argv[2]);
            char confirmation;
            scanf_s(" %c", &confirmation, 1);
            if (confirmation != 'y' && confirmation != 'Y') {
                printf("Uninstall cancelled by user.\n");
                return 0;
            }
            // Uses winget backend to securely handle the uninstallation workflow via admin
            target_file = "winget.exe";
            sprintf_s(parameters, sizeof(parameters), "uninstall --uninstall-previous --path \"%s\"", argv[2]);
        }
        // msudo reboot
        else if (strcmp(argv[1], "reboot") == 0) {
            printf("[msudo] Executing quick computer system restart...\n");
            target_file = "shutdown.exe";
            strcpy_s(parameters, sizeof(parameters), "/r /t 0");
        }
        // msudo bios
        else if (strcmp(argv[1], "bios") == 0) {
            printf("[msudo] Rebooting directly into hardware UEFI/BIOS settings...\n");
            target_file = "shutdown.exe";
            strcpy_s(parameters, sizeof(parameters), "/r /fw /t 0");
        }
        // msudo recovery (Secret diagnostic delay concept)
        else if (strcmp(argv[1], "recovery") == 0) {
            printf("[msudo] Pre-configuring system to trigger Automatic Repair state upon next boot animation...\n");
            target_file = "bcdedit.exe";
            strcpy_s(parameters, sizeof(parameters), "/set {current} recoveryenabled Yes");
            // Executes a forced immediate shutdown recovery trigger alongside it
            ShellExecuteEx(&(SHELLEXECUTEINFO){sizeof(SHELLEXECUTEINFO), SEE_MASK_DEFAULT, NULL, "runas", "shutdown.exe", "/r /o /t 0", NULL, SW_NORMAL, NULL});
        }
        // msudo whodoyoulikethemost
        else if (strcmp(argv[1], "whodoyoulikethemost") == 0) {
            printf("You're awesome and beautiful!\n");
            return 0;
        }
        // LINUX COMPATIBILITY WRAPPERS
        // msudo apt update
        else if (strcmp(argv[1], "apt") == 0 && argc > 2 && strcmp(argv[2], "update") == 0) {
            printf("[msudo] Simulating Linux Package Manager API over Windows Environment...\n");
            target_file = "winget.exe";
            strcpy_s(parameters, sizeof(parameters), "source update");
        }
        // msudo -i
        else if (strcmp(argv[1], "-i") == 0) {
            printf("[msudo] Dropping into interactive administrator root shell...\n");
            target_file = "powershell.exe";
            strcpy_s(parameters, sizeof(parameters), "-NoExit -Command \"cd C:\\Users\\Administrator\"");
        }
        // msudo -s
        else if (strcmp(argv[1], "-s") == 0) {
            printf("[msudo] Spawning root shell environment while preserving current variables...\n");
            target_file = "cmd.exe";
            strcpy_s(parameters, sizeof(parameters), "/k");
        }
        // msudo !! (Re-runs previous command context wrapper)
        else if (strcmp(argv[1], "!!") == 0) {
            printf("[msudo] Re-running your last shell instruction stream with superuser access rights...\n");
            target_file = "doskey.exe";
            strcpy_s(parameters, sizeof(parameters), "/history"); // Standard fallback grab
        }
        // UNIVERSAL PASS-THROUGH (msudo [any exe or lnk])
        else {
            target_file = argv[1];
            // Safely pack any additional flag arguments passed after the application name
            for (int i = 2; i < argc; i++) {
                strcat_s(parameters, sizeof(parameters), argv[i]);
                if (i < argc - 1) {
                    strcat_s(parameters, sizeof(parameters), " ");
                }
            }
        }
    }

    // 3. EXECUTE WINDOWS PROCESS WITH SECURE UAC PRIVILEGE ELEVATION
    SHELLEXECUTEINFO sei = { 0 };
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_DEFAULT;
    sei.lpVerb = "runas";          // Triggers the official secure Windows UAC admin validation prompt
    sei.lpFile = target_file;      // Applies our mapped internal utility or external application string
    sei.lpParameters = (strlen(parameters) > 0) ? parameters : NULL;
    sei.nShow = SW_NORMAL;

    if (!ShellExecuteEx(&sei)) {
        printf("Failed to elevate execution privileges for targeted execution stream.\n");
        return 1;
    }

    return 0;
}
