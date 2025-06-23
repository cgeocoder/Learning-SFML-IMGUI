#pragma once

#ifndef __FUNCS_H__
#define __FUNCS_H__

#define _CRT_SECURE_NO_WARNINGS

#include <imgui/imgui.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <filesystem>

namespace ACW {
    void reload_window_enum();
	std::vector<std::pair<std::string, std::string>>& get_window_enum();
    std::vector<std::pair<std::string, std::string>> get_window_enum_coincidences(std::string str);

    void delete_target(size_t index);
    void add_target(std::string tname = "", int tdelay = 0, bool use = true);
    
    class ACWTarget {
    public:
        ACWTarget(std::string tname = "", int tdelay = -1, bool tuse = false);

        char* m_TargetName;
        int m_Delay;
        bool m_Using;
    };

    class ACWDir {
    public:
        ACWDir();

        void back();
        void reset_dir();
        void reset(bool dir_reset = false);
        void set_dir(std::string new_dir);

        std::string m_DirName;
        std::vector<std::filesystem::directory_entry> m_Ents;
    };

    std::vector<ACW::ACWTarget>& get_acw_targets();
    ACWDir& get_acwdir();

    void draw_menu(float& font_zoom);
    void save_config();
    void load_config();
}

#endif // !__FUNCS_H__