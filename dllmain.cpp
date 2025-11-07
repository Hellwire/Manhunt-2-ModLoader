#define LOWORD(_dw)         ((WORD)(((DWORD_PTR)(_dw)) & 0xffff))
#define HIWORD(_dw)         ((WORD)((((DWORD_PTR)(_dw)) >> 16) & 0xffff))
#define LODWORD(_qw)        ((INT)(_qw))
#define HIDWORD(_qw)        ((DWORD)(((_qw) >> 32) & 0xffffffff))

#include "ManhuntSdk.h"
#include <stdarg.h>
#include <algorithm>
#include "menu.h"
#include <string>
#include <iostream>
#include <fstream>
#include "calling.h"
#include "MemoryMgr.h"
#include <filesystem>
#include <vector>
#include <limits>
#include <conio.h>
#include <thread>
#include <chrono>
#include <stddef.h>
#include <set>


namespace fs = std::filesystem;

#include <cctype>
static std::vector<std::string> activeMods; // load order: first -> last; last has highest priority

static inline bool HasActiveMods() { return !activeMods.empty(); }

static std::string JoinActiveModsDisplay() {
    if (activeMods.empty()) return "regular game";
    std::string s;
    for (size_t i = 0; i < activeMods.size(); ++i) {
        if (i) s += " -> ";
        s += activeMods[i];
    }
    return s;
}

// Disable Legal Screen
// store the original values from memory before we patch them.
static int  original_val_53FC68;
static int  original_val_53FC50;
static int  original_val_53FC6F;
static char original_val_6596FC;
static bool legalScreenOriginalsSaved = false;

struct CConfig
{
    // existing UM2 settings
    int flash = 0;
    int cameraShake = 0;
    int funmode = 0;
    int whitenoise = 0;
    int SkipModPrompt = 0;
    int disableLegalScreen = 0;
    int bloodStay = 0;
    int tvpCamera = 0;    // 0=0.01,1=PC,2=PS2 Leak,3=PS2/PSP,4=Wii
    int debug = 0;        // debug setting hidden from menu. maybe stupid idk
};

void RestoreIntroFunctionality()
{
    std::this_thread::sleep_for(std::chrono::seconds(8));

    if (legalScreenOriginalsSaved)
    {
        Memory::VP::Patch<int>(0x53FC68, original_val_53FC68);
        Memory::VP::Patch<int>(0x53FC50, original_val_53FC50);
        Memory::VP::Patch<int>(0x53FC6F, original_val_53FC6F);
        Memory::VP::Patch<char>(0x6596FC, original_val_6596FC);
    }
}

void __fastcall SelectMod() {
    std::ifstream configFile("settings.um2");
    std::string path = "mods/";
    std::vector<std::string> modList;

    std::cout << "Ultimate Manhunt 2 Modloader by Sor3nt & Hellwire" << std::endl;
    std::cout << "Version 1.4.6 REV 05/11/25" << std::endl;
    std::cout << std::endl << std::endl;

    std::cout << "[UM2] Please select a Mod by pressing the corresponding key:" << std::endl << std::endl;
    std::cout << "[0] Regular game" << std::endl;

    if (!fs::exists(path)) {
        std::cout << std::endl;
        std::cout << "[UM2] No Mods folder found! Please create a \"mods/\" folder." << std::endl;
        std::cout << "[UM2] Press any key to continue with the regular game..." << std::endl;
        _getch();
        activeMods.clear();
        return;
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) modList.push_back(entry.path().filename().string());
    }
    if (modList.empty()) {
        std::cout << std::endl;
        std::cout << "[UM2] No Mods found! Please place the mods inside the \"mods/\" folder." << std::endl;
        std::cout << "Press any key to continue with the regular game..." << std::endl;
        _getch();
        activeMods.clear();
        return;
    }

    // List first 8 for quick single-pick, reserve 9 for multi-sequence
    size_t quickCount = (std::min)((size_t)8, modList.size());
    for (size_t i = 0; i < quickCount; ++i) {
        std::cout << "[" << (i + 1) << "] " << modList[i] << std::endl;
    }
    if (modList.size() > 8) {
        std::cout << "..." << (modList.size() - 8) << " more found." << std::endl;
    }
    std::cout << "[9] Multi-mod selection" << std::endl;

    int maxChoice = 9; // 0..8 single, 9 = multi
    std::string prompt = "Waiting for input..." + std::to_string((int)quickCount);
    std::cout << std::endl << prompt;
    std::cout.flush();

    int selected = -1;
    while (true) {
        if (_kbhit()) {
            char ch = _getch();
            int choice = ch - '0';
            if (choice == 0 || (choice >= 1 && choice <= (int)quickCount) || choice == 9) {
                selected = choice;
                break;
            }
            else {
                std::cout << "\rError: wrong input. Please press a valid key.                               ";
                std::cout.flush();
                Sleep(500);
                std::cout << "\r" << prompt << "                                                                   ";
                std::cout << "\r" << prompt;
                std::cout.flush();
            }
        }
        Sleep(30);
    }

    system("cls");

    std::cout << "Ultimate Manhunt 2 Modloader by Sor3nt & Hellwire" << std::endl;
    std::cout << "Version 1.4.6 REV 05/11/25" << std::endl;
    std::cout << std::endl << std::endl;

    if (selected == 0) {
        activeMods.clear();
        std::cout << "Regular game selected. Launching..." << std::endl;
        return;
    }

    if (selected >= 1 && selected <= (int)quickCount) {
        activeMods.clear();
        activeMods.push_back(modList[(size_t)selected - 1]);
        std::cout << "Mod selected: " << activeMods.back() << ". Launching..." << std::endl;
        return;
    }

    // Show full list to allow any index
    std::cout << "[UM2] Multi-mod loader mode" << std::endl << std::endl;
    for (size_t i = 0; i < modList.size(); ++i) {
        std::cout << "[" << (i + 1) << "] " << modList[i] << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Enter a sequence of mods to load using indices Example: 1,3,4" << std::endl;
    std::cout << "Input: ";
    std::cout.flush();

    std::string line;
    // make sure we read the fresh line
    std::getline(std::cin, line);
    if (line.empty()) std::getline(std::cin, line);

    // Parse any non-digit as separator
    std::vector<int> indices;
    int current = -1;
    bool inNumber = false;
    auto flushNumber = [&]() {
        if (inNumber && current >= 0) {
            if (current >= 1 && current <= (int)modList.size())
                indices.push_back(current);
        }
        current = -1; inNumber = false;
        };

    for (char c : line) {
        if (std::isdigit((unsigned char)c)) {
            int d = c - '0';
            if (!inNumber) { current = d; inNumber = true; }
            else { current = current * 10 + d; }
        }
        else {
            flushNumber();
        }
    }
    flushNumber();

    // Build ordered, de-duplicated mod chain
    std::vector<std::string> chain;
    for (int idx : indices) {
        const std::string& m = modList[(size_t)idx - 1];
        if (std::find(chain.begin(), chain.end(), m) == chain.end())
            chain.push_back(m);
    }

    if (chain.empty()) {
        std::cout << "[UM2] No valid indices entered. Booting regular game." << std::endl;
        activeMods.clear();
        return;
    }

    activeMods = std::move(chain);
    std::cout << "Mods selected (load order, last wins): " << JoinActiveModsDisplay() << std::endl;
}

void DummyReturnVoid() {
    printf("Dummy Called");
    return;
}

CConfig config;

void ResetConfig() {
    config.flash = 0;
    config.cameraShake = 0;
    config.funmode = 0;
    config.whitenoise = 0;
    config.SkipModPrompt = 0;
    config.disableLegalScreen = 0;
    config.bloodStay = 0;
    config.tvpCamera = 0;
    config.debug = 0;
}

void UpdateDisableLegalScreen() {
    // intentionally empty; patched at startup in InitializeASI
}

void UpdateWhitenoise() {
    if (config.whitenoise == 1) {
        Memory::VP::InjectHook(0x5C5CED, DummyReturnVoid, PATCH_CALL);
        Memory::VP::Patch<char>(0x5AF74F + 1, 0x00);
        Memory::VP::Patch<char>(0x5AF734 + 1, 0x00);
    }
    else {
        Memory::VP::Patch<char>(0x5C5CED, 0xE8);
        Memory::VP::Patch<char>(0x5C5CED + 1, 0x9E);
        Memory::VP::Patch<char>(0x5C5CED + 2, 0x9A);
        Memory::VP::Patch<char>(0x5C5CED + 3, 0xFE);
        Memory::VP::Patch<char>(0x5C5CED + 4, 0xFF);
        Memory::VP::Patch<char>(0x5AF74F + 1, 0x01);
        Memory::VP::Patch<char>(0x5AF734 + 1, 0x01);
    }
}

void UpdateFunMode() {
    if (config.funmode == 1) {
        *(int*)0x76BE40 = 32;
        *(char*)0x6B26E5 = 0;
    }
    else {
        *(int*)0x76BE40 = 0;
        *(char*)0x6B26E5 = 1;
    }
}

void UpdateCameraShake() {
    if (config.cameraShake == 1) {
        Memory::VP::Patch<char>(0x5956CA + 1, 0x85);
    }
    else {
        Memory::VP::Patch<char>(0x5956CA + 1, 0x84);
    }
}

void UpdateFlash() {
    int colRampName = *(int*)0x75C0A8;
    colRampName = colRampName + 0x208;
    if (config.flash == 1) {
        // Break the colramp name so it cannot load it anymore
        Memory::VP::Patch<char>(colRampName + 2, 0x00);
    }
    else {
        // Restore the colramp name
        Memory::VP::Patch<char>(colRampName + 2, 0x5f);
    }
}

void SaveSettings() {
    std::ofstream myfile;
    myfile.open("settings.um2");
    myfile << "flash=" << config.flash << "\n";
    myfile << "cameraShake=" << config.cameraShake << "\n";
    myfile << "funmode=" << config.funmode << "\n";
    myfile << "whitenoise=" << config.whitenoise << "\n";
    myfile << "SkipModPrompt=" << config.SkipModPrompt << "\n";
    myfile << "disableLegalScreen=" << config.disableLegalScreen << "\n";
    myfile << "bloodStay=" << config.bloodStay << "\n";
    myfile << "tvpCamera=" << config.tvpCamera << "\n";
    myfile << "debug=" << config.debug << "\n";
    myfile.close();
}

bool settingsLoaded = false;
bool settingsFlashLoaded = false;
void ReadSettings() {
    std::ifstream fin("settings.um2");
    std::string line;
    while (std::getline(fin, line)) {
        size_t eqPos = line.find("=");
        if (eqPos == std::string::npos) continue;

        std::string attr = line.substr(0, eqPos);
        std::string valueStr = line.substr(eqPos + 1);
        try {
            int value = std::stoi(valueStr);

            if (attr == "cameraShake") {
                config.cameraShake = value;
            }
            else if (attr == "funmode") {
                config.funmode = value;
            }
            else if (attr == "whitenoise") {
                config.whitenoise = value;
            }
            else if (attr == "flash") {
                config.flash = value;
            }
            else if (attr == "SkipModPrompt") {
                config.SkipModPrompt = value;
            }
            else if (attr == "disableLegalScreen") {
                config.disableLegalScreen = value;
            }
            else if (attr == "bloodStay") {
                config.bloodStay = value;
            }
            else if (attr == "tvpCamera") {
                config.tvpCamera = value;
            }
            else if (attr == "debug") {
                config.debug = value;
            }
        }
        catch (const std::invalid_argument&) {
            std::cerr << "[UM2] Warning: Invalid value for '" << attr << "' in settings.um2" << std::endl;
        }
    }
}

void ApplySettings() {
    UpdateCameraShake();
    UpdateFunMode();
    UpdateWhitenoise();
    // UpdateFlash() runs later after level load to avoid crashes
}

void Init() {
    while (true) {
        Sleep(1);
        if (*(CSDKEntity**)0x789490 && !settingsFlashLoaded) {
            UpdateFlash();
            settingsFlashLoaded = true;
        }
    }
}

int endCase = 0x55F31D;
int command;
char* commandText;

void __cdecl HandleNeoMenuChangeLanguageLoadMap(int a1, int a2) {
    char* cmd = (char*)(*(int*)a2);
    printf("Neo cmd %s\n", cmd);
    if (strcmp(cmd, "setLanguage(chinese)") == 0) {
        printf("1");
        int set = *(int*)0x7604C8;
        ((int(__fastcall*)(int, int, int))0x56C0A0)((int)&set, 1, 1);
        printf("2");
        int v4 = *(int*)0x75F110;
        printf("3");
        ((int(__fastcall*)(int))0x562AB0)((int)v4);
        printf("4");
        ((void(__fastcall*)(int, int*, char))0x562B00)(v4, (int*)(a1 + 120), 1);
        printf("5");
    }
    else {
        ((void(__cdecl*)(int, int))0x55F1A0)(a1, a2);
    }
}

// Handle custom NeoMenu string commands for TVP selection
void __cdecl HandleNeoMenuCommands(int a1, int a2) {
    char* cmd = (char*)(*(int*)a2);
    printf("HandleNeoMenuCommands %s\n", cmd);

    bool isCustomCmd = false;

    if (strcmp(cmd, "tvp001") == 0 || strcmp(cmd, "tvpOriginal") == 0) { isCustomCmd = true; config.tvpCamera = 0; }
    else if (strcmp(cmd, "tvpPC") == 0 || strcmp(cmd, "tvpStatic") == 0) { isCustomCmd = true; config.tvpCamera = 1; }
    else if (strcmp(cmd, "tvpPs2Leak") == 0) { isCustomCmd = true; config.tvpCamera = 2; }
    else if (strcmp(cmd, "tvpPs2Psp") == 0) { isCustomCmd = true; config.tvpCamera = 3; }
    else if (strcmp(cmd, "tvpWii") == 0) { isCustomCmd = true; config.tvpCamera = 4; }
    else if (strcmp(cmd, "set default settings") == 0) {
        isCustomCmd = true; ResetConfig();
    }

    if (isCustomCmd) {
        // send apply msg
        ((void(__thiscall*)(int*, int*, int))0x562B00)((int*)0x75F110, (int*)(a1 + 120), 1);
        SaveSettings();
        return;
    }

    ((void(__cdecl*)(int, int))0x55CC60)(a1, a2);
}

bool EnsureSettingsFileIntegrity()
{
    static const std::vector<std::string> kRequired = {
        "flash",
        "cameraShake",
        "funmode",
        "whitenoise",
        "SkipModPrompt",
        "disableLegalScreen",
        "bloodStay",
        "tvpCamera",
        "debug",
    };

    const char* kPath = "settings.um2";

    auto trim = [](std::string s) {
        const char* ws = " \t\r\n";
        s.erase(0, s.find_first_not_of(ws));
        s.erase(s.find_last_not_of(ws) + 1);
        return s;
        };

    bool needsRebuild = false;

    if (!fs::exists(kPath)) {
        needsRebuild = true;
    }
    else {
        std::ifstream fin(kPath);
        if (!fin.is_open()) {
            needsRebuild = true;
        }
        else {
            std::set<std::string> seen;
            std::string line;
            while (std::getline(fin, line)) {
                if (line.empty()) continue;
                size_t eq = line.find('=');
                if (eq == std::string::npos) { needsRebuild = true; break; }

                std::string key = trim(line.substr(0, eq));
                std::string val = trim(line.substr(eq + 1));

                // ignore unknown keys
                if (std::find(kRequired.begin(), kRequired.end(), key) == kRequired.end())
                    continue;

                try {
                    int v = std::stoi(val);
                    if (key == "tvpCamera") {
                        if (v < 0 || v > 4) { needsRebuild = true; break; }
                    }
                    else {
                        if (v != 0 && v != 1) { needsRebuild = true; break; }
                    }
                }
                catch (...) {
                    needsRebuild = true; break;
                }

                seen.insert(key);
            }

            // any missing key -> rebuild
            for (const auto& req : kRequired) {
                if (!seen.count(req)) { needsRebuild = true; break; }
            }
        }
    }

    if (needsRebuild) {
        std::error_code ec;
        fs::remove(kPath, ec); // best-effort delete
        ResetConfig();         // restore defaults
        SaveSettings();        // write a fresh file
        std::cout << "[UM2] Rebuilt settings.um2 with defaults.\n";
        return true;
    }

    return false;
}

int __fastcall WrapGetNeoMenuValue(int ptr, int a2, int a3) {
    char* fieldName = *(char**)a3;
    if (strcmp(fieldName, "cameraShake") == 0)      return config.cameraShake;
    else if (strcmp(fieldName, "SkipModPrompt") == 0) return config.SkipModPrompt;
    else if (strcmp(fieldName, "funmode") == 0)      return config.funmode;
    else if (strcmp(fieldName, "whitenoise") == 0)   return config.whitenoise;
    else if (strcmp(fieldName, "flash") == 0)        return config.flash;
    else if (strcmp(fieldName, "disableLegalScreen") == 0) return config.disableLegalScreen;
    else if (strcmp(fieldName, "bloodStay") == 0)    return config.bloodStay;
    else if (strcmp(fieldName, "tvpCamera") == 0)    return config.tvpCamera;
    else if (strcmp(fieldName, "debug") == 0)        return config.debug;

    return ((int(__thiscall*)(int, int))0x562510)(ptr, a3);
}

int __fastcall UpdateNeoMenuActiveStates(int ptr, int a2, int namePtr, int status) {
    char* fieldName = *(char**)namePtr;
    if (strcmp(fieldName, "funmode") == 0) {
        config.funmode = status; UpdateFunMode();
    }
    else if (strcmp(fieldName, "cameraShake") == 0) {
        config.cameraShake = status; UpdateCameraShake();
    }
    else if (strcmp(fieldName, "flash") == 0) {
        config.flash = status; UpdateFlash();
    }
    else if (strcmp(fieldName, "SkipModPrompt") == 0) {
        config.SkipModPrompt = status;
    }
    else if (strcmp(fieldName, "whitenoise") == 0) {
        config.whitenoise = status; UpdateWhitenoise();
    }
    else if (strcmp(fieldName, "disableLegalScreen") == 0) {
        config.disableLegalScreen = status; UpdateDisableLegalScreen();
    }
    else if (strcmp(fieldName, "bloodStay") == 0) {
        config.bloodStay = status;
    }
    else if (strcmp(fieldName, "tvpCamera") == 0) {
        config.tvpCamera = status; // note: normally set via string commands too
    }
    else if (strcmp(fieldName, "debug") == 1) {
        config.debug = status;
    }

    SaveSettings();
    return ((int(__thiscall*)(int, int, int))0x562630)(ptr, namePtr, status);
}

char* GetReplacementName(char* filename) {
    return filename;
}

int __cdecl WrapReadBinary(char* filename) {
    auto callOriginal = [](const std::string& p) -> int {
        return ((int(__cdecl*)(char*))0x53BCC0)((char*)p.c_str());
        };

    auto ends_with_icase = [](const std::string& s, const char* suf) -> bool {
        const size_t n = strlen(suf);
        if (s.size() < n) return false;
        for (size_t i = 0; i < n; ++i) {
            char a = (char)tolower((unsigned char)s[s.size() - n + i]);
            char b = (char)tolower((unsigned char)suf[i]);
            if (a != b) return false;
        }
        return true;
        };

    std::string original(filename ? filename : "");
    std::replace(original.begin(), original.end(), '\\', '/');
    if (original.rfind("./", 0) == 0)      original = original.substr(2);
    else if (original.rfind("/", 0) == 0)  original = original.substr(1);

    // TVP swap for resource13.glg
    if (original == "global/ini/resource13.glg") {
        std::string alt;
        switch (config.tvpCamera) {
        case 2: alt = "global/ini/resource13_ps2leak.glg";  break;
        case 3: alt = "global/ini/resource13_ps2psp.glg";   break;
        case 4: alt = "global/ini/resource13_wii.glg";      break;
        default: break;
        }

        if (!alt.empty()) {
            // Check active mods in reverse so last selected wins
            for (int i = (int)activeMods.size() - 1; i >= 0; --i) {
                std::string act = "mods/" + activeMods[(size_t)i] + "/" + alt;
                if (fs::exists(act)) {
                    if (config.debug) std::cout << "[UM2] TVP: " << alt << " from: " << act << std::endl;
                    return callOriginal(act);
                }
            }
            // loose fallback
            std::string loose = "mods/" + alt;
            if (fs::exists(loose)) {
                if (config.debug) std::cout << "[UM2] TVP: " << alt << " from loose mods: " << loose << std::endl;
                return callOriginal(loose);
            }
        }
    }

    if (!HasActiveMods()) {
        return callOriginal(original);
    }

    // Build candidate list evaluated per mod in reverse priority
    // 1) exact mirror
    for (int i = (int)activeMods.size() - 1; i >= 0; --i) {
        std::string cand = "mods/" + activeMods[(size_t)i] + "/" + original;
        if (fs::exists(cand)) {
            if (config.debug) std::cout << "[UM2] Replace: " << original << " -> " << cand << std::endl;
            return callOriginal(cand);
        }
    }

    // 2) Bink extras
    if (ends_with_icase(original, ".bik")) {
        const std::string base = fs::path(original).filename().string();
        for (int i = (int)activeMods.size() - 1; i >= 0; --i) {
            const std::string& m = activeMods[(size_t)i];
            const std::string c1 = "mods/" + m + "/movies/" + base;
            const std::string c2 = "mods/" + m + "/video/" + base;
            const std::string c3 = "mods/" + m + "/" + base;
            if (fs::exists(c1)) { if (config.debug) std::cout << "[UM2] Replace: " << original << " -> " << c1 << std::endl; return callOriginal(c1); }
            if (fs::exists(c2)) { if (config.debug) std::cout << "[UM2] Replace: " << original << " -> " << c2 << std::endl; return callOriginal(c2); }
            if (fs::exists(c3)) { if (config.debug) std::cout << "[UM2] Replace: " << original << " -> " << c3 << std::endl; return callOriginal(c3); }
        }
    }

    return callOriginal(original);
}

static std::string Normalize(std::string s) {
    std::replace(s.begin(), s.end(), '\\', '/');
    if (s.rfind("./", 0) == 0) s = s.substr(2);
    else if (s.rfind("/", 0) == 0) s = s.substr(1);
    return s;
}

static bool EndsWithICase(const std::string& s, const char* suf) {
    const size_t n = strlen(suf);
    if (s.size() < n) return false;
    for (size_t i = 0; i < n; ++i) {
        char a = (char)tolower((unsigned char)s[s.size() - n + i]);
        char b = (char)tolower((unsigned char)suf[i]);
        if (a != b) return false;
    }
    return true;
}

// Returns empty if no override found; otherwise returns absolute mod path to use
// === REPLACE ResolveBinkOverride with this version ===
static std::string ResolveBinkOverride(const std::string& original) {
    if (!HasActiveMods()) return {};

    // 1) exact mirror across mods in reverse
    for (int i = (int)activeMods.size() - 1; i >= 0; --i) {
        std::string p = "mods/" + activeMods[(size_t)i] + "/" + original;
        if (fs::exists(p)) return p;
    }

    // 2) bink-specific extras
    if (EndsWithICase(original, ".bik")) {
        const std::string base = fs::path(original).filename().string();
        for (int i = (int)activeMods.size() - 1; i >= 0; --i) {
            const std::string& m = activeMods[(size_t)i];
            const std::string c1 = "mods/" + m + "/movies/" + base;
            const std::string c2 = "mods/" + m + "/video/" + base;
            const std::string c3 = "mods/" + m + "/" + base;
            if (fs::exists(c1)) return c1;
            if (fs::exists(c2)) return c2;
            if (fs::exists(c3)) return c3;
        }
    }

    return {};
}

static void(__cdecl* Real_PrepMoviePath)(char*) = (void(__cdecl*)(char*))0x53ABA0;

// The buffer in BinkMovieLoader looks like a 0x100-byte stack buffer.
// Stay within that limit and bail if replacement is too long.
void __cdecl WrapPrepMoviePath(char* path) {
    if (!path) { Real_PrepMoviePath(path); return; }

    std::string orig = Normalize(path);
    std::string repl = ResolveBinkOverride(orig);

    if (!repl.empty()) {
        // Max safe write incl. NUL
        constexpr size_t kBufCap = 0x100;

        // Windows APIs accept both '/' and '\', but make it stock-like
        std::replace(repl.begin(), repl.end(), '/', '\\');

        if (repl.size() + 1 <= kBufCap) {
#ifdef _MSC_VER
            strncpy_s(path, kBufCap, repl.c_str(), _TRUNCATE);
#else
            std::snprintf(path, kBufCap, "%s", repl.c_str());
#endif
            std::cout << "[UM2] Bink path overridden: " << orig << " -> " << repl << std::endl;
        }
        else {
            std::cout << "[UM2] Bink override too long (" << repl.size()
                << " bytes), skipping override for: " << orig << std::endl;
        }
    }

    // Call the real function (no recursion since we’re only hooking the callsite, not the entry)
    Real_PrepMoviePath(path);
}


// Timed Vector Pair inline adjuster for tvpCamera=1 (PC/Static)
int __fastcall SetVecPairValues(int input, int a, int data) {
    int vecPairs = ((int(__thiscall*)(int, int))0x53B920)(input, data);
    if (config.tvpCamera != 0) {
        *(float*)(data + 36) = 5.5f;
    }
    return vecPairs;
}

// === REPLACE Wrap2ReadBinary with this version ===
FILE* __cdecl Wrap2ReadBinary(char* filename, char* mode) {
    static char moddedPathBuffer[1024];

    if (!HasActiveMods()) {
        return ((FILE * (__cdecl*)(char*, char*, int))0x61B338)(filename, mode, 64);
    }

    // Check mods in reverse so last in sequence wins
    for (int i = (int)activeMods.size() - 1; i >= 0; --i) {
        std::string modFile = "mods/" + activeMods[(size_t)i] + "/" + filename;
        std::ifstream f(modFile.c_str());
        if (f.good()) {
            std::cout << "[UM2] Replace file: " << filename << " -> " << modFile << std::endl;
            strcpy_s(moddedPathBuffer, sizeof(moddedPathBuffer), modFile.c_str());
            return ((FILE * (__cdecl*)(char*, char*, int))0x61B338)(moddedPathBuffer, mode, 64);
        }
    }

    return ((FILE * (__cdecl*)(char*, char*, int))0x61B338)(filename, mode, 64);
}

unsigned int __cdecl GetTexturByName(char* textureName) {
    printf("textureName: %s", textureName);
    return ((unsigned int(__cdecl*)(char*))0x57A190)(textureName);
}

int __fastcall sub_5ED660(int mem, int count1, int count2, int pedHeadAndOrPlayerBitmask, int bitModel4, int a6, int a7, int a8, int a9, float a10, float a11, float a12, float a13, float a14, int a15, float a16) {
    printf("Count 2: %i\n", count2);
    printf("a6: %i\n", a6);
    printf("a7: %i\n", a7);
    return ((int(__thiscall*)(int, int, int, int, int, int, int, int, int, float, float, float, float, float, int, float))0x5ED660)(mem, count1, count2, pedHeadAndOrPlayerBitmask, bitModel4, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16);
}

int __fastcall GetHudWeaponIcon(int graphicPtr, int a, int weaponId) {
    return *(int*)(graphicPtr + 984);
}

void HandleHudWeaponIcon() { /* unchanged from UM2; keeping stub to avoid size */ }

int __cdecl CalcHash(char const* str)
{
    int hash = 0;
    for (char const* c = str; *c; ++c) {
        char chr = (unsigned char)(*c - 97) <= 25 ? *c - 32 : *c;
        hash = hash * 33 + chr;
    }
    std::ofstream myfile;
    myfile.open("hashes.txt", std::fstream::out | std::fstream::app);
    myfile << str << "\n";
    myfile.close();
    return hash;
}

int __fastcall crc32(int* self, int a, char* a2)
{
    char* v2; // edi@1
    char v3; // al@1
    unsigned int v4; // esi@1
    int* i; // ebx@1

    std::ofstream myfile;
    myfile.open("crc32.txt", std::fstream::out | std::fstream::app);
    myfile << a2 << "\n";
    myfile.close();

    v2 = a2;
    v3 = *a2;
    v4 = -1;
    for (i = self; v3; ++v2)
    {
        v4 = i[(unsigned __int8)(v4 ^ tolower(v3))] ^ (v4 >> 8);
        v3 = v2[1];
    }
    return ~v4;
}

// Returns the address of the CALL instruction inside BinkMovieLoader&InputHandoff that calls 0x53ABA0
// so we can replace it with a call to WrapPrepMoviePath.
uintptr_t FindCallToPrepMoviePath() {
    // The function ends at 0x4B08A5; start a bit earlier to cover the body.
    // You can widen/narrow this range as needed; 0x200 bytes is plenty here.
    const uintptr_t scanStart = 0x4B0700;
    const size_t    scanLen = 0x200;
    const uintptr_t target = 0x53ABA0;

    const uint8_t* p = reinterpret_cast<const uint8_t*>(scanStart);
    for (size_t i = 0; i + 5 <= scanLen; ++i) {
        if (p[i] == 0xE8) { // CALL rel32
            int32_t rel = *reinterpret_cast<const int32_t*>(p + i + 1);
            uintptr_t dest = (scanStart + i + 5) + rel;
            if (dest == target) {
                return scanStart + i; // address of the CALL we want to detour
            }
        }
    }
    return 0;
}


extern "C"
{
    __declspec(dllexport) void InitializeASI()
    {
        AllocConsole();
        SetConsoleTitleA("Ultimate Manhunt 2 Mod");
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        EnsureSettingsFileIntegrity();    // may recreate file using those defaults

        ReadSettings();
        // --- Handle Legal Screen Skip on Startup (with timed restore) ---
        if (config.disableLegalScreen == 1)
        {
            if (!legalScreenOriginalsSaved) {
                original_val_53FC68 = *(int*)0x53FC68;
                original_val_53FC50 = *(int*)0x53FC50;
                original_val_53FC6F = *(int*)0x53FC6F;
                original_val_6596FC = *(char*)0x6596FC;
                legalScreenOriginalsSaved = true;
            }
            Memory::VP::Patch<int>(0x53FC68, 0);
            Memory::VP::Patch<int>(0x53FC50, 0);
            Memory::VP::Patch<int>(0x53FC6F, 0);
            Memory::VP::Patch<char>(0x6596FC, 0x00);
            std::thread(RestoreIntroFunctionality).detach();
            std::cout << "[UM2] Intro and legal screen skip activated." << std::endl;
        }

        if (config.SkipModPrompt == 1) {
            std::cout << "[UM2] Modloader skipped. Booting regular game." << std::endl;
            activeMods.clear();
        }
        else {
            SelectMod();
            if (!HasActiveMods()) {
                std::cout << "[UM2] No Mod selected. Booting regular game." << std::endl;
            }
            else {
                std::cout << "Boot Mods: " << JoinActiveModsDisplay() << std::endl;
            }

            ApplySettings();

            printf("[UM2] Hooking file access for mod loading... ");

            if (auto callPrep = FindCallToPrepMoviePath()) {
                Memory::VP::InjectHook(callPrep, WrapPrepMoviePath, PATCH_CALL);
                std::cout << "[UM2] Hooked PrepMoviePath at callsite 0x" << std::hex << callPrep << std::dec << "\n";
            }
            else {
                std::cout << "[UM2] WARNING: Could not find callsite to 0x53ABA0; Bink override will not apply.\n";
            }
            Memory::VP::InjectHook(0x57B2E5, Wrap2ReadBinary, PATCH_CALL);
            // 53CC17 is FSB LOAD AUDIO
            Memory::VP::InjectHook(0x53CC17, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x418A1E, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4B0842, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4C3A7C, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4C7F68, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4C8D87, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4CCD10, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4CD186, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4CD245, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4CFEB7, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4E9029, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x4EBB04, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x53BD0B, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x53EA07, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x56AEE9, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x56E550, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x583EC4, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x586249, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x5BC856, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x5BC86B, WrapReadBinary, PATCH_CALL);
            Memory::VP::InjectHook(0x5DEA76, WrapReadBinary, PATCH_CALL);
            printf("OK\n");
        }

        printf("[UM2] Hooking neomenu functions... ");
        Memory::VP::InjectHook(0x560078, HandleNeoMenuChangeLanguageLoadMap, PATCH_CALL);
        Memory::VP::InjectHook(0x560069, HandleNeoMenuCommands, PATCH_CALL); // ADDED
        Memory::VP::InjectHook(0x5570B1, WrapGetNeoMenuValue, PATCH_CALL);
        Memory::VP::InjectHook(0x5571E6, WrapGetNeoMenuValue, PATCH_CALL);
        Memory::VP::InjectHook(0x55DFA0, WrapGetNeoMenuValue, PATCH_CALL);
        Memory::VP::InjectHook(0x55E58E, WrapGetNeoMenuValue, PATCH_CALL);
        Memory::VP::InjectHook(0x5625B6, WrapGetNeoMenuValue, PATCH_CALL);
        Memory::VP::InjectHook(0x55DEAA, UpdateNeoMenuActiveStates, PATCH_CALL);
        Memory::VP::InjectHook(0x55E4B6, UpdateNeoMenuActiveStates, PATCH_CALL);
        Memory::VP::InjectHook(0x55E721, UpdateNeoMenuActiveStates, PATCH_CALL);
        printf("OK\n");

        printf("[UM2] Patching game functions... ");
        // rename DIE pose 3/2 to 4 to avoid missing anims
        Memory::VP::Patch<char>(0x6423B4, 0x34);
        Memory::VP::Patch<char>(0x6423D8, 0x34);
        Memory::VP::Patch<char>(0x642320, 0x34);
        Memory::VP::Patch<char>(0x642344, 0x34);
        Memory::VP::Patch<char>(0x523A9C + 2, 0x1e); // brain bits boost
        // Patch blood decals to stay forever
        Memory::VP::Nop(0x005E4C1F, 5);
        //Memory::VP::Patch<char>(0x5E4C10, 0x74); // Fix for decals getting removed when paused but breaks game, needs different solution
		//Memory::VP::Patch<char>(0x5E4388, 0x75); //kept here for reference
        //Memory::VP::Patch<char>(0x5E42C8, 0x75)
        printf("OK\n");

        // Hook Timed Vector Pair adjustments (needed for tvpCamera=1 static mode)
        Memory::VP::InjectHook(0x5CB3FC, SetVecPairValues, PATCH_CALL);
        Memory::VP::InjectHook(0x5CB445, SetVecPairValues, PATCH_CALL);

        printf("[UM2] Starting game thread... ");
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Init), nullptr, 0, nullptr);
        printf("OK\n");
    }
}
