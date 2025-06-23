#pragma once

#ifndef __VARS_H__
#define __VARS_H__

#include <fstream>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#define ACW_MINOR_VERSION "0"
#define ACW_MAJOR_VERSION "2"

#ifdef _DEBUG
# define ACW_VERSION "(Debug v" ACW_MAJOR_VERSION "." ACW_MINOR_VERSION ")"
#else
# define ACW_VERSION "(Release v" ACW_MAJOR_VERSION "." ACW_MINOR_VERSION ")"
#endif 

namespace ACW {

	constexpr const int TARGET_BUF_SIZE = 127;
	constexpr const int DEFAULT_WINDOW_HEIGHT = 650;
	constexpr int DEFAULT_WINDOW_WIDTH = 950;
	constexpr const ImVec2 window_size = ImVec2(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

	constexpr const char* MAIN_WINDOW_NAME = "AutoCloseWindow visual config " ACW_VERSION;

#ifdef _DEBUG
	constexpr const char* CONFIG_FILENAME = "debug_config.acw";
#else
	constexpr const char* CONFIG_FILENAME = ".config.acw";
#endif 

	constexpr const ImGuiWindowFlags WINDOW_FLAGS =
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
	
	constexpr const ImGuiTextFlags TARGET_INPUT_FLAGS = 
		ImGuiInputTextFlags_EnterReturnsTrue 
		| ImGuiInputTextFlags_CallbackEdit 
		| ImGuiInputTextFlags_CallbackHistory;

	constexpr const ImGuiTableFlags TARGETS_TABLE_FLAGS =
		ImGuiTableFlags_Resizable
		| ImGuiTableFlags_Borders
		| ImGuiTableColumnFlags_NoResize;

	constexpr const int IMGUI_INIT_FAILED_EXIT = 0x1;
	constexpr const int OK_EXIT = 0x0;
}

#endif // !__VARS_H__