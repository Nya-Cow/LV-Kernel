#include <map>
#include "imgui_Custom.h"
#include <unordered_map>

namespace Animations {

		int FastIntLerp(const char* identifier, bool state, int min, int max, int speed) {

			static std::unordered_map<const char*, int> valuesMapInt;
			auto value = valuesMapInt.find(identifier);

			if (value == valuesMapInt.end()) {
				valuesMapInt.insert({ identifier,  min });
				value = valuesMapInt.find(identifier);
			}

			const float frameRateSpeed = speed * (1.f - ImGui::GetIO().DeltaTime);

			if (state) {
				if (value->second < max)
					value->second += frameRateSpeed;
			}
			else {
				if (value->second > min)
					value->second -= frameRateSpeed;
			}

			value->second = std::clamp(value->second, min, max);

			return value->second;
		}

		float FastFloatLerp(std::string identifier, bool state, float min, float max, float speed) {

			static std::unordered_map<std::string, float> valuesMapFloat;
			auto value = valuesMapFloat.find(identifier);

			if (value == valuesMapFloat.end()) {
				valuesMapFloat.insert({ identifier, min });
				value = valuesMapFloat.find(identifier);
			}

			const float frameRateSpeed = speed * (1.f - ImGui::GetIO().DeltaTime);

			if (state) {
				if (value->second < max)
					value->second += frameRateSpeed;
			}
			else {
				if (value->second > min)
					value->second -= frameRateSpeed;
			}

			value->second = std::clamp(value->second, min, max);

			return value->second;
		}

		ImVec2 FastImVec2Lerp(const char* identifier, bool state, ImVec2 min, ImVec2 max, float speed) {

			static std::unordered_map<const char*, ImVec2> valuesMapFloat;
			auto value = valuesMapFloat.find(identifier);

			if (value == valuesMapFloat.end()) {
				valuesMapFloat.insert({ identifier, min });
				value = valuesMapFloat.find(identifier);
			}

			const float frameRateSpeed = speed * (1.f - ImGui::GetIO().DeltaTime);

			if (state) {
				if (value->second.x < max.x)
					value->second.x += frameRateSpeed;

				if (value->second.y < max.y)
					value->second.y += frameRateSpeed;
			}
			else {
				if (value->second.x > min.x)
					value->second.x -= frameRateSpeed;

				if (value->second.y > min.y)
					value->second.y -= frameRateSpeed;
			}

			value->second.x = std::clamp(value->second.x, min.x, max.x);
			value->second.y = std::clamp(value->second.y, min.y, max.y);

			return value->second;
		}

		ImColor FastColorLerp(ImColor start, ImColor end, float stage)
		{
			ImVec4 lerp = ImLerp(
				ImVec4(start.Value.x, start.Value.y, start.Value.z, start.Value.w),
				ImVec4(end.Value.x, end.Value.y, end.Value.z, end.Value.w),
				stage);

			return ImGui::ColorConvertFloat4ToU32(lerp);
		}
	}

void ImGui::My_SwitchButton(const char* str_id, bool* v)
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = 35;
    float width = height * 2.2f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *v = !*v;

    ImGuiContext& gg = *GImGui;
    ImColor Radia = colors[ImGuiCol_CheckMark];
    float ANIM_SPEED = 0.085f;

    if (gg.LastActiveId == gg.CurrentWindow->GetID(str_id)) // && g.LastActiveIdTimer < ANIM_SPEED)
    {
        float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
        // 这里可以添加动画逻辑（如果需要）
    }

    if (ImGui::IsItemHovered())
    {
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? ImVec4(0.17f, 0.17f, 0.17f, 0.4f) : ImVec4(0.17f, 0.17f, 0.17f, 0.4f)), height * 0.5f);
    }
    else
    {
        Radia = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? ImVec4(0.17f, 0.17f, 0.17f, 0.4f) : ImVec4(0.17f, 0.17f, 0.17f, 0.4f)), height * 0.50f);
    }

    if (*v)
    {
        Radia = colors[ImGuiCol_CheckMark];
    }
    else
    {
        Radia = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f), p.y + radius - 1.5f), radius - 10.f, Radia);

    // 设置文本颜色为白色
    ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec2 text_pos = ImVec2(p.x + width * 1.25f, p.y + radius - 15);
    PushStyleColor(ImGuiCol_Text, white);
    RenderText(text_pos, str_id);
    PopStyleColor();
}
