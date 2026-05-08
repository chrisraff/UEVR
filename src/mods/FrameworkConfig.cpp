#include "Framework.hpp"

#include "FrameworkConfig.hpp"

std::shared_ptr<FrameworkConfig>& FrameworkConfig::get() {
     static std::shared_ptr<FrameworkConfig> instance{std::make_shared<FrameworkConfig>()};
     return instance;
}

std::optional<std::string> FrameworkConfig::on_initialize() {
    return Mod::on_initialize();
}

void FrameworkConfig::draw_main() {
    m_menu_key->draw("Menu Key");
    m_show_cursor_key->draw("Show Cursor Key");
    m_remember_menu_state->draw("Remember Menu Open/Closed State");
    m_enable_l3_r3_toggle->draw("Enable L3 + R3 Toggle");
    ImGui::SameLine();
    m_l3_r3_long_press->draw("L3 + R3 Long Press Menu Toggle");
    m_always_show_cursor->draw("Always Show Cursor");

    ImGui::Separator();
    if (m_log_level->draw("Log Level")) {
        if (m_log_level->value() >= 0 && m_log_level->value() <= spdlog::level::level_enum::n_levels) {
            spdlog::set_level((spdlog::level::level_enum)m_log_level->value());   
        }
    }
}

void FrameworkConfig::draw_themes() {
    get_imgui_theme()->draw("Select GUI Theme");

    if (m_font_size->draw("Font Size")) {
        g_framework->set_font_size(m_font_size->value());
    }
}

void FrameworkConfig::draw_nav_bindings() {
    ImGui::TextWrapped("Bind buttons from any input device (keyboard, gamepad, racing wheel, joystick, etc.) to navigate this menu.");
    ImGui::TextWrapped("Click a button below, then press the button you want to bind. Right-click to clear.");
    ImGui::Separator();

    m_nav_up->draw("Navigate Up");
    m_nav_down->draw("Navigate Down");
    m_nav_left->draw("Navigate Left");
    m_nav_right->draw("Navigate Right");
    ImGui::Separator();
    m_nav_confirm->draw("Confirm / Select");
    m_nav_cancel->draw("Cancel / Back");
    ImGui::Separator();
    m_nav_prev_tab->draw("Previous Tab (L1)");
    m_nav_next_tab->draw("Next Tab (R1)");
    ImGui::Separator();
    m_nav_tweak_slow->draw("Tweak Value Slowly (L2)");
    m_nav_tweak_fast->draw("Tweak Value Quickly (R2)");
}

void FrameworkConfig::on_draw_sidebar_entry(std::string_view in_entry) {
    on_draw_ui();
    ImGui::Separator();

    if (in_entry == "Main") {
        draw_main();
    } else if (in_entry == "GUI/Themes") {
        draw_themes();
    } else if (in_entry == "Nav Bindings") {
        draw_nav_bindings();
    }
}

void FrameworkConfig::on_pre_imgui_frame() {
    if (!g_framework->is_drawing_ui()) {
        return;
    }

    auto& io = ImGui::GetIO();

    auto inject = [&](ImGuiKey key, bool down, bool& prev) {
        if (down || prev) {
            io.AddKeyEvent(key, down);
            prev = down;
        }
    };

    inject(ImGuiKey_GamepadDpadUp,    m_nav_up->is_key_down(),         m_nav_prev_state.up);
    inject(ImGuiKey_GamepadDpadDown,  m_nav_down->is_key_down(),       m_nav_prev_state.down);
    inject(ImGuiKey_GamepadDpadLeft,  m_nav_left->is_key_down(),       m_nav_prev_state.left);
    inject(ImGuiKey_GamepadDpadRight, m_nav_right->is_key_down(),      m_nav_prev_state.right);
    inject(ImGuiKey_GamepadFaceDown,  m_nav_confirm->is_key_down(),    m_nav_prev_state.confirm);
    inject(ImGuiKey_GamepadFaceRight, m_nav_cancel->is_key_down(),     m_nav_prev_state.cancel);
    inject(ImGuiKey_GamepadL1,        m_nav_prev_tab->is_key_down(),   m_nav_prev_state.prev_tab);
    inject(ImGuiKey_GamepadR1,        m_nav_next_tab->is_key_down(),   m_nav_prev_state.next_tab);
    inject(ImGuiKey_GamepadL2,        m_nav_tweak_slow->is_key_down(), m_nav_prev_state.tweak_slow);
    inject(ImGuiKey_GamepadR2,        m_nav_tweak_fast->is_key_down(), m_nav_prev_state.tweak_fast);
}

void FrameworkConfig::on_frame() {
    if (m_menu_key->is_key_down_once()) {
        g_framework->set_draw_ui(!g_framework->is_drawing_ui(), true);
    }

    if (m_show_cursor_key->is_key_down_once()) {
        m_always_show_cursor->toggle();
    }
}

void FrameworkConfig::on_config_load(const utility::Config& cfg, bool set_defaults) {
    for (IModValue& option : m_options) {
        option.config_load(cfg, set_defaults);
    }

    if (m_remember_menu_state->value()) {
        g_framework->set_draw_ui(m_menu_open->value(), false);
    }
    
    g_framework->set_font_size(m_font_size->value());

    if (m_log_level->value() >= 0 && m_log_level->value() <= spdlog::level::level_enum::n_levels) {
        spdlog::set_level((spdlog::level::level_enum)m_log_level->value());   
    }
}

void FrameworkConfig::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}
