#ifndef IMGUI_CUSTOMIZE_H
#define IMGUI_CUSTOMIZE_H

#include "imgui.h"
#include "imgui_internal.h"
#include <string>

namespace Animations {

		/*
		// Usage:
		//  int trickInt = ImTricks::Animations::FastLerpInt("trickInt", enable_animation, 0, 255, 15);
		//  draw->AddRectFilled(p, p + ImVec2(30, 30), ImColor(255, 255, 255, trickInt));
		*/
		extern int FastIntLerp(const char* identifier, bool state, int min, int max, int speed);

		/*
		// Usage:
		//  float trickFloat = ImTricks::Animations::FastLerpInt("trickFloat", enable_animation, 0.f, 1.f, 0.05f);
		//  draw->AddRectFilled(p, p + ImVec2(30, 30), ImColor(1.f, 1.f, 1.f, trickFloat));
		*/
		extern float FastFloatLerp(std::string identifier, bool state, float min, float max, float speed);


		/*
		// Usage:
		//  float trickFloat = ImTricks::Animations::FastFloatLerp("header", enable_animation, 0.f, 1.f, 0.05f);
		//  ImVec2 pos1 = ImTricks::Animations::FastImVec2Lerp("pos1", enable_animation, ImVec2(0, 50), ImVec2(100, 50), 5);
		//  ImVec2 pos2 = ImTricks::Animations::FastImVec2Lerp("pos2", enable_animation, ImVec2(413, 80), ImVec2(513, 80), 5);
		//  draw->AddRectFilled(p + pos1, p + pos2, ImTricks::Animations::FastColorLerp(ImColor(255, 0, 0), ImColor(0, 255, 0), trickFloat));
		*/
		extern ImVec2 FastImVec2Lerp(const char* identifier, bool state, ImVec2 min, ImVec2 max, float speed);

		/*
		// Usage:
		//	float trickFloat = ImTricks::Animations::FastLerpFloat("header", check, 0.f, 1.f, 0.05f);
		//	draw->AddRectFilled(p, p + ImVec2(513, 30), ImTricks::Animations::FastColorLerp(ImColor(255, 0, 0), ImColor(0, 255, 0), trickFloat));
		*/
		extern ImColor FastColorLerp(ImColor start, ImColor end, float stage);
	}

namespace ImGui {
    //自定义::(控件)小组件
    IMGUI_API void          My_SwitchButton(const char* str_id, bool* v);


    IMGUI_API bool          M_ani_Checkbox(const char* label, bool* v, const ImVec2& size = ImVec2(50, 50), float fontSize_times = 1.0f);

}

#endif //IMGUI_CUSTOMIZE_H
