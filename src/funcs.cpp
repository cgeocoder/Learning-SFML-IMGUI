#include "funcs.h"
#include "vars.h"

namespace ACW {

    static std::vector<std::pair<std::string, std::string>> gWindowNames;
    std::vector<ACW::ACWTarget> gTargets;
    static ACWDir gDir;

    std::vector<std::pair<std::string, std::string>> get_window_enum_coincidences(std::string str) {
        using enum_window_vec = std::vector<std::pair<std::string, std::string>>;

        enum_window_vec res;

        for (size_t j = 0; j < gWindowNames.size(); j++) {
            std::string window_name = gWindowNames.at(j).first;

            if (window_name.find(str) != std::string::npos) {
                res.push_back(gWindowNames.at(j));
            }
        }

        return res;
    }

    ACWDir& get_acwdir() {
        return gDir;
    }

    std::vector<ACW::ACWTarget>& get_acw_targets() {
        return gTargets;
    }

    void delete_target(size_t index) {
        if (char* ptr = gTargets.at(index).m_TargetName)
            delete[] ptr;

        gTargets.erase(gTargets.begin() + index);
    }

    void add_target(std::string tname, int tdelay, bool use) {
        gTargets.push_back(ACW::ACWTarget(tname, tdelay, use));
    }

    static BOOL CALLBACK window_enum_callback(HWND hwnd, LPARAM lParam) {
        char title[127] = { 0 };
        GetWindowTextA(hwnd, title, sizeof(title));

        char fname[127] = { 0 };
        GetWindowModuleFileNameA(hwnd, fname, sizeof(fname));

        if (title[0])
            gWindowNames.push_back(std::pair(title, fname));

        return 1;
    }

    void reload_window_enum() {
        gWindowNames.clear();
        EnumWindows(window_enum_callback, NULL);
    }

    std::vector<std::pair<std::string, std::string>>& get_window_enum() {
        return gWindowNames;
    }

    ACWTarget::ACWTarget(std::string tname, int tdelay, bool tuse) {
        m_TargetName = new char[ACW::TARGET_BUF_SIZE];

        std::strcpy(m_TargetName, tname.c_str());

        m_Delay = tdelay;
        m_Using = tuse;
    }

    void ACWDir::set_dir(std::string new_dir) {
        m_DirName = new_dir;
        reset();
    }

    void ACWDir::back() {
        m_DirName = m_DirName.substr(0, m_DirName.find_last_of("\\"));
        reset();
    }

    ACWDir::ACWDir() {
        reset(true);
    }

    void ACWDir::reset_dir() {
        CHAR buffer[MAX_PATH] = { 0 };
        GetModuleFileNameA(NULL, buffer, MAX_PATH);

        m_DirName = std::string(buffer).substr(0, std::string(buffer).find_last_of("\\"));
    }

    void ACWDir::reset(bool dir_reset) {
        if (dir_reset) {
            reset_dir();
        }

        m_Ents.clear();

        try {
            for (const auto& entry : std::filesystem::directory_iterator(m_DirName + "\\")) {
                m_Ents.push_back(entry);
            }
        }
        catch (std::exception ex) {}
    }

    void draw_menu(float& font_zoom) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Save")) {
                if (ImGui::MenuItem("Save this config", "Ctrl+S")) {
                    ACW::save_config();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("New")) {
                if (ImGui::MenuItem("Add new target", "Ctrl+N")) {
                    ACW::add_target();
                    ACW::reload_window_enum();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Scale")) {
                if (ImGui::MenuItem("More", "Ctrl+", nullptr, font_zoom < 2.1f)) {
                    font_zoom += 0.1f;
                }

                if (ImGui::MenuItem("Less", "Ctrl-", nullptr, font_zoom > 1.1f)) {
                    font_zoom -= 0.1f;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About program")) {


                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    void save_config() {
        std::ofstream config_file{ ACW::CONFIG_FILENAME };

        for (ACW::ACWTarget& t : gTargets) {
            config_file << t.m_TargetName
                << "|" << t.m_Delay 
                << "|" << t.m_Using << '\n';
        }

        config_file.close();
    }

    void load_config() {
        std::ifstream config_file{ ACW::CONFIG_FILENAME };

        std::string line;
        while (getline(config_file, line)) {
            size_t sep1 = line.find_first_of("|"),
                sep2 = line.find_last_of("|");

            try {
                std::string name = line.substr(0, sep1);
                std::string delay = line.substr(sep1 + 1, sep2 - sep1);
                std::string use = line.substr(sep2 + 1);

                gTargets.push_back(
                    ACW::ACWTarget(name, std::stoi(delay), (bool)stoi(use))
                );
            }
            catch (std::exception ex) {
                MessageBoxA(
                    NULL,
                    "load_config() failed"
                    "\n[?] Failed while load config from file",

                    "AutoCloseWindow: config error",
                    MB_ICONERROR | MB_OKCANCEL);

                return;
            }
        }

        config_file.close();
    }
}