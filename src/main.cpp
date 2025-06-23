#define _CRT_SECURE_NO_WARNINGS

#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <filesystem>

#include "vars.h"
#include "funcs.h"

#include <Dwmapi.h>

#pragma comment (lib, "Dwmapi.lib")

float title_bar_size_y = 0.0f;

std::vector<std::pair<std::string, std::string>> win_enum_coincidences;
size_t tooltip_colored_index = 0;

int reload_callback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        ACW::reload_window_enum();
        win_enum_coincidences = ACW::get_window_enum_coincidences((char*)data->Buf);
    }
    else if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
        if (data->EventKey == ImGuiKey_UpArrow) {
            if (tooltip_colored_index > 0)
                tooltip_colored_index -= 1;
            else
                tooltip_colored_index = win_enum_coincidences.size() - 1;
        }
        else if (data->EventKey == ImGuiKey_DownArrow) {
            tooltip_colored_index = ((tooltip_colored_index + 1) % win_enum_coincidences.size());
        }
    }

    return 0;
}

int main() {
#ifndef _DEBUG
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif // _DEBUG

    ACW::load_config();

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{ ACW::DEFAULT_WINDOW_WIDTH, ACW::DEFAULT_WINDOW_HEIGHT }),
        "AutoCloseWindow " ACW_VERSION, sf::Style::None);

    window.setFramerateLimit(60);

    MARGINS margins { -1 };

    SetWindowLongA(window.getNativeHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
    DwmExtendFrameIntoClientArea(window.getNativeHandle(), &margins);

    if (!ImGui::SFML::Init(window)) {
        MessageBoxA(
            window.getNativeHandle(),
            "ImGui::SFML::Init() failed"
            "\n[?] Failed initialize ImGUI with SFML window"
            "\n[+] Check the versions and the required DLL"
            "\n[+] Check the Debug/Release version and the version of the application",

            "AutoCloseWindow: Initialization error",
            MB_ICONSTOP | MB_OK);

        return ACW::IMGUI_INIT_FAILED_EXIT;
    }

    float font_zoom = 1.5f, wheel_counter = 0.0f;

    ACW::reload_window_enum();
    win_enum_coincidences = ACW::get_window_enum_coincidences("");

    sf::Vector2i grabbedOffset;
    bool window_focused = true;
    auto window_move_dispatcher = [&]() -> void {
        if (window_focused && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (ImGui::GetMousePos().y <= title_bar_size_y) {
                window.setPosition(sf::Mouse::getPosition() + grabbedOffset);
            }
        }

        grabbedOffset = window.getPosition() - sf::Mouse::getPosition();
    };

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            else if (event->is<sf::Event::FocusLost>()) {
                window_focused = false;
            }
            
            else if (event->is<sf::Event::FocusGained>()) {
                window_focused = true;
            }

            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                // Save HK
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                    ACW::save_config();
                }

                // New HK
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N)) {
                    ACW::add_target();
                    ACW::reload_window_enum();
                }

                // Zoom in HK
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Equal)) {
                    if (font_zoom < 2.1f)
                        font_zoom += 0.1f;
                }

                // Zoom out HK
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Hyphen)) {
                    if (font_zoom > 1.1f)
                        font_zoom -= 0.1f;
                }
            }
        }

        window_move_dispatcher();

        ImGui::SFML::Update(window, deltaClock.restart());

        float old_size = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= font_zoom;
        ImGui::PushFont(ImGui::GetFont());

       {
            // Main Window
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(window.getSize());

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 40.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin(ACW::MAIN_WINDOW_NAME, nullptr, ACW::WINDOW_FLAGS);

            ImGui::PopStyleVar(3);

            title_bar_size_y = ACW::DEFAULT_WINDOW_HEIGHT - ImGui::GetContentRegionAvail().y;

            ACW::draw_menu(font_zoom);

            //  1 |    2   |   3    |   4   |   5    
            // ID | Status | Target | Delay | Delete

            if (ImGui::BeginTable("Targets_Table", 5, ACW::TARGETS_TABLE_FLAGS)) {

                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch, 0.2f);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch, 0.4f);
                ImGui::TableSetupColumn("Target");
                ImGui::TableSetupColumn("Millisec", ImGuiTableColumnFlags_WidthStretch, 0.2f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.35f);

                ImGui::TableHeadersRow();

                auto& targets = ACW::get_acw_targets();
                for (size_t i = 0; i < targets.size(); ++i) {
                    // ID
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(i).c_str());

                    // Status
                    ImGui::TableNextColumn();
                    std::string checkbox_title =
                        (targets[i].m_Using ? "use" : "ignore") +
                        (std::string)"##Checkbox" + std::to_string(i);

                    ImGui::Checkbox(checkbox_title.c_str(), &targets[i].m_Using);

                    // Target
                    ImGui::TableNextColumn();
                    std::string input_text_id = "##InputText" + std::to_string(i);

                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());

                    bool input_event = ImGui::InputText(
                        input_text_id.c_str(),
                        targets[i].m_TargetName,
                        ACW::TARGET_BUF_SIZE,
                        ACW::TARGET_INPUT_FLAGS,
                        reload_callback);

                    if (input_event) {
                        std::strcpy(
                            targets[i].m_TargetName,
                            win_enum_coincidences.at(tooltip_colored_index).first.c_str());

                        tooltip_colored_index = 0;
                    }

                    // Target input hints
                    if (ImGui::GetID(input_text_id.c_str()) == ImGui::GetActiveID()) {
                        ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetItemRectMax().y + 5.0f));
                        ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y));

                        ImGui::BeginTooltip();

                        ImGui::SetScrollY(wheel_counter -= ImGui::GetIO().MouseWheel * 30.0f);

                        for (size_t j = 0; j < win_enum_coincidences.size(); j++) {
                            ImVec4 colored_text = (tooltip_colored_index == j) ? ImVec4(1, 1, 0, 1) : ImVec4(1, 1, 1, 1);

                            ImGui::TextColored(
                                colored_text,
                                ("[Name] " + win_enum_coincidences.at(j).first).c_str());
                            ImGui::TextColored(
                                colored_text,
                                ("[Path] " + (win_enum_coincidences.at(j).second.empty() ? "?" : win_enum_coincidences.at(j).second)).c_str());

                            ImGui::NewLine();
                        }

                        ImGui::EndTooltip();
                    }

                    // Delay
                    ImGui::TableNextColumn();

                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                    ImGui::InputInt(("##InputInt" + std::to_string(i)).c_str(), &targets[i].m_Delay, ImGuiInputTextFlags_None);

                    // Delete
                    ImGui::TableNextColumn();

                    bool button_pressed = ImGui::Button(
                        ("Delete##Button" + std::to_string(i)).c_str(),
                        ImVec2(ImGui::GetColumnWidth(), ImGui::GetColumnOffset()));

                    if (button_pressed) {
                        ACW::delete_target(i);
                        break;
                    }
                }

                ImGui::EndTable();
            }

            if (ImGui::Button("Add new target")) {
                ACW::add_target();
            }

            ImGui::End();
        }

        ImGui::GetFont()->Scale = old_size;
        ImGui::PopFont();

        window.clear(sf::Color::Transparent);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return ACW::OK_EXIT;
}