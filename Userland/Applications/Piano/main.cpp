/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2019-2020, William McPherson <willmcpherson2@gmail.com>
 * Copyright (c) 2021, kleines Filmröllchen <filmroellchen@serenityos.org>
 * Copyright (c) 2021, JJ Roberts-White <computerfido@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "AudioPlayerLoop.h"
#include "MainWidget.h"
#include "TrackManager.h"
#include <AK/Atomic.h>
#include <AK/Queue.h>
#include <LibAudio/ConnectionToServer.h>
#include <LibAudio/WavWriter.h>
#include <LibCore/EventLoop.h>
#include <LibCore/System.h>
#include <LibGUI/Action.h>
#include <LibGUI/Application.h>
#include <LibGUI/FilePicker.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Menubar.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Window.h>
#include <LibMain/Main.h>
#include <LibThreading/MutexProtected.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio thread proc rpath cpath wpath recvfd sendfd unix"));

    auto app = TRY(GUI::Application::try_create(arguments));

    TrackManager track_manager;

    Threading::MutexProtected<Audio::WavWriter> wav_writer;
    Optional<DeprecatedString> save_path;
    Atomic<bool> need_to_write_wav = false;

    auto audio_loop = AudioPlayerLoop::construct(track_manager, need_to_write_wav, wav_writer);

    auto app_icon = GUI::Icon::default_icon("app-piano"sv);
    auto window = GUI::Window::construct();
    auto main_widget = TRY(window->try_set_main_widget<MainWidget>(track_manager, audio_loop));
    window->set_title("Piano");
    window->resize(840, 600);
    window->set_icon(app_icon.bitmap_for_size(16));

    auto main_widget_updater = Core::Timer::construct(static_cast<int>((1 / 30.0) * 1000), [&] {
        if (window->is_active())
            Core::EventLoop::current().post_event(main_widget, make<Core::CustomEvent>(0));
    });
    main_widget_updater->start();

    auto& file_menu = window->add_menu("&File");
    file_menu.add_action(GUI::Action::create("Export", { Mod_Ctrl, Key_E }, TRY(Gfx::Bitmap::try_load_from_file("/res/icons/16x16/file-export.png"sv)), [&](const GUI::Action&) {
        save_path = GUI::FilePicker::get_save_filepath(window, "Untitled", "wav");
        if (!save_path.has_value())
            return;
        DeprecatedString error;
        wav_writer.with_locked([&](auto& wav_writer) {
            wav_writer.set_file(save_path.value());
            if (wav_writer.has_error()) {
                error = DeprecatedString::formatted("Failed to export WAV file: {}", wav_writer.error_string());
                wav_writer.clear_error();
            }
        });
        if (!error.is_empty()) {
            GUI::MessageBox::show_error(window, error);
            return;
        }
        need_to_write_wav = true;
    }));
    file_menu.add_separator();
    file_menu.add_action(GUI::CommonActions::make_quit_action([](auto&) {
        GUI::Application::the()->quit();
        return;
    }));

    auto& edit_menu = window->add_menu("&Edit");
    main_widget->add_track_actions(edit_menu);

    auto& help_menu = window->add_menu("&Help");
    help_menu.add_action(GUI::CommonActions::make_command_palette_action(window));
    help_menu.add_action(GUI::CommonActions::make_about_action("Piano", app_icon, window));

    window->show();

    return app->exec();
}
