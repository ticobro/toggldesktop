// Copyright 2014 Toggl Desktop developers.

// No exceptions should be thrown from this library.

#include "./../src/toggl_api.h"

#include <cstring>
#include <set>

#include "./toggl_api_lua.h"

#include "./client.h"
#include "./const.h"
#include "./context.h"
#include "./custom_error_handler.h"
#include "./feedback.h"
#include "./formatter.h"
#include "./https_client.h"
#include "./project.h"
#include "./proxy.h"
#include "./time_entry.h"
#include "./timeline_uploader.h"
#include "./toggl_api_private.h"
#include "./user.h"
#include "./websocket_client.h"
#include "./window_change_recorder.h"

#include "Poco/Bugcheck.h"
#include "Poco/Path.h"
#include "Poco/Logger.h"
#include "Poco/UnicodeConverter.h"

void *toggl_context_init(
    const char_t *app_name,
    const char_t *app_version) {
    poco_check_ptr(app_name);
    poco_check_ptr(app_version);

    toggl::Context *ctx =
        new toggl::Context(to_string(app_name), to_string(app_version));

    return ctx;
}

bool_t toggl_ui_start(void *context) {
    return toggl::noError == app(context)->StartEvents();
}

void toggl_password_forgot(void *context) {
    app(context)->UI()->DisplayURL(kLostPasswordURL);
}

void toggl_context_clear(void *context) {
    if (context) {
        app(context)->SetQuit();
        app(context)->Shutdown();
    }
    delete app(context);
}

bool_t toggl_set_settings_remind_days(
    void *context,
    const bool_t remind_mon,
    const bool_t remind_tue,
    const bool_t remind_wed,
    const bool_t remind_thu,
    const bool_t remind_fri,
    const bool_t remind_sat,
    const bool_t remind_sun) {

    return toggl::noError == app(context)->SetSettingsRemindDays(
        remind_mon,
        remind_tue,
        remind_wed,
        remind_thu,
        remind_fri,
        remind_sat,
        remind_sun);
}

bool_t toggl_set_settings_remind_times(
    void *context,
    const char_t *remind_starts,
    const char_t *remind_ends) {

    poco_check_ptr(remind_starts);
    poco_check_ptr(remind_ends);

    return toggl::noError == app(context)->SetSettingsRemindTimes(
        to_string(remind_starts),
        to_string(remind_ends));
}

bool_t toggl_set_settings_use_idle_detection(
    void *context,
    const bool_t use_idle_detection) {
    return toggl::noError == app(context)->
           SetSettingsUseIdleDetection(use_idle_detection);
}

bool_t toggl_set_settings_autotrack(
    void *context,
    const bool_t value) {
    return toggl::noError == app(context)->SetSettingsAutotrack(value);
}

bool_t toggl_set_settings_open_editor_on_shortcut(
    void *context,
    const bool_t value) {
    return toggl::noError ==
           app(context)->SetSettingsOpenEditorOnShortcut(value);
}

bool_t toggl_set_settings_autodetect_proxy(
    void *context,
    const bool_t autodetect_proxy) {
    return toggl::noError == app(context)->
           SetSettingsAutodetectProxy(autodetect_proxy);
}

bool_t toggl_set_settings_menubar_timer(
    void *context,
    const bool_t menubar_timer) {
    return toggl::noError == app(context)->
           SetSettingsMenubarTimer(menubar_timer);
}

bool_t toggl_set_settings_menubar_project(
    void *context,
    const bool_t menubar_project) {
    return toggl::noError == app(context)->
           SetSettingsMenubarProject(menubar_project);
}

bool_t toggl_set_settings_dock_icon(
    void *context,
    const bool_t dock_icon) {
    return toggl::noError == app(context)->SetSettingsDockIcon(dock_icon);
}

bool_t toggl_set_settings_on_top(
    void *context,
    const bool_t on_top) {
    return toggl::noError == app(context)->SetSettingsOnTop(on_top);
}

bool_t toggl_set_settings_reminder(
    void *context,
    const bool_t reminder) {
    return toggl::noError == app(context)->SetSettingsReminder(reminder);
}

bool_t toggl_set_settings_idle_minutes(
    void *context,
    const uint64_t idle_minutes) {
    return toggl::noError == app(context)->SetSettingsIdleMinutes(idle_minutes);
}

bool_t toggl_set_settings_focus_on_shortcut(
    void *context,
    const bool_t focus_on_shortcut) {
    return toggl::noError == app(context)->
           SetSettingsFocusOnShortcut(focus_on_shortcut);
}

bool_t toggl_set_settings_manual_mode(
    void *context,
    const bool_t manual_mode) {
    return toggl::noError == app(context)->SetSettingsManualMode(manual_mode);
}

bool_t toggl_set_settings_reminder_minutes(
    void *context,
    const uint64_t reminder_minutes) {
    return toggl::noError == app(context)->
           SetSettingsReminderMinutes(reminder_minutes);
}

bool_t toggl_set_window_settings(
    void *context,
    const int64_t window_x,
    const int64_t window_y,
    const int64_t window_height,
    const int64_t window_width) {

    return toggl::noError == app(context)->SaveWindowSettings(
        window_x,
        window_y,
        window_height,
        window_width);
}

bool_t toggl_window_settings(
    void *context,
    int64_t *window_x,
    int64_t *window_y,
    int64_t *window_height,
    int64_t *window_width) {

    poco_check_ptr(context);

    poco_check_ptr(window_x);
    poco_check_ptr(window_y);
    poco_check_ptr(window_height);
    poco_check_ptr(window_width);

    return toggl::noError == app(context)->LoadWindowSettings(
        window_x,
        window_y,
        window_height,
        window_width);
}

bool_t toggl_set_proxy_settings(void *context,
                                const bool_t use_proxy,
                                const char_t *proxy_host,
                                const uint64_t proxy_port,
                                const char_t *proxy_username,
                                const char_t *proxy_password) {
    poco_check_ptr(proxy_host);
    poco_check_ptr(proxy_username);
    poco_check_ptr(proxy_password);

    toggl::Proxy proxy;
    proxy.SetHost(to_string(proxy_host));
    proxy.SetPort(proxy_port);
    proxy.SetUsername(to_string(proxy_username));
    proxy.SetPassword(to_string(proxy_password));

    return toggl::noError == app(context)->SetProxySettings(use_proxy, proxy);
}

void toggl_set_cacert_path(
    void *context,
    const char_t *path) {
    poco_check_ptr(path);

    toggl::HTTPSClient::Config.CACertPath = to_string(path);
}

bool_t toggl_set_db_path(
    void *context,
    const char_t *path) {
    poco_check_ptr(path);

    return toggl::noError == app(context)->SetDBPath(to_string(path));
}

void toggl_set_update_path(
    void *context,
    const char_t *path) {
    poco_check_ptr(path);

    return app(context)->SetUpdatePath(to_string(path));
}

char_t *toggl_update_path(
    void *context) {
    return copy_string(app(context)->UpdatePath());
}


void toggl_set_environment(
    void *context,
    const char_t *environment) {
    poco_check_ptr(environment);

    return app(context)->SetEnvironment(to_string(environment));
}

char_t *toggl_environment(
    void *context) {
    return copy_string(app(context)->Environment());
}

void toggl_disable_update_check(
    void *context) {

    app(context)->DisableUpdateCheck();
}

void toggl_set_log_path(const char_t *path) {
    poco_check_ptr(path);

    toggl::Context::SetLogPath(to_string(path));
}

void toggl_set_log_level(const char_t *level) {
    poco_check_ptr(level);

    Poco::Logger::get("").setLevel(to_string(level));
}

void toggl_show_app(
    void *context) {
    app(context)->UI()->DisplayApp();
}

bool_t toggl_login(
    void *context,
    const char_t *email,
    const char_t *password) {

    poco_check_ptr(email);
    poco_check_ptr(password);

    return toggl::noError == app(context)->Login(to_string(email),
            to_string(password));
}

bool_t toggl_signup(
    void *context,
    const char_t *email,
    const char_t *password) {

    poco_check_ptr(email);
    poco_check_ptr(password);

    return toggl::noError == app(context)->Signup(to_string(email),
            to_string(password));
}

bool_t toggl_google_login(
    void *context,
    const char_t *access_token) {

    poco_check_ptr(access_token);

    return toggl::noError == app(context)->GoogleLogin(to_string(access_token));
}

bool_t toggl_logout(
    void *context) {

    logger().debug("toggl_logout");

    return toggl::noError == app(context)->Logout();
}

bool_t toggl_clear_cache(
    void *context) {

    logger().debug("toggl_clear_cache");

    return toggl::noError == app(context)->ClearCache();
}

void toggl_sync(void *context) {
    logger().debug("toggl_sync");
    app(context)->Sync();
}

char_t *toggl_create_project(
    void *context,
    const uint64_t workspace_id,
    const uint64_t client_id,
    const char_t *project_name,
    const bool_t is_private) {

    toggl::Project *p = app(context)->CreateProject(
        workspace_id,
        client_id,
        "",
        to_string(project_name),
        is_private);

    if (p) {
        return copy_string(p->GUID());
    }
    return nullptr;
}

char_t *toggl_create_client(
    void *context,
    const uint64_t workspace_id,
    const char_t *client_name) {

    toggl::Client *c = app(context)->CreateClient(
        workspace_id,
        to_string(client_name));

    if (!c) {
        return nullptr;
    }

    poco_check_ptr(c);

    return copy_string(c->GUID());
}

char_t *toggl_add_project(
    void *context,
    const char_t *time_entry_guid,
    const uint64_t workspace_id,
    const uint64_t client_id,
    const char_t *client_guid,
    const char_t *project_name,
    const bool_t is_private) {

    poco_check_ptr(time_entry_guid);

    toggl::Project *p = app(context)->CreateProject(
        workspace_id,
        client_id,
        to_string(client_guid),
        to_string(project_name),
        is_private);
    if (!p) {
        return nullptr;
    }

    poco_check_ptr(p);

    char_t *guid = copy_string(p->GUID());
    toggl_set_time_entry_project(
        context,
        time_entry_guid,
        0, /* no task ID */
        p->ID(),
        guid);

    return guid;
}

char_t *toggl_format_tracking_time_duration(
    const int64_t duration_in_seconds) {

    std::string formatted = toggl::Formatter::FormatDuration(
        duration_in_seconds, toggl::Format::Classic);

    return copy_string(formatted);
}

char_t *toggl_format_tracked_time_duration(
    const int64_t duration_in_seconds) {

    std::string formatted = toggl::Formatter::FormatDuration(
        duration_in_seconds, toggl::Format::Improved, false);

    return copy_string(formatted);
}

char_t *toggl_start(
    void *context,
    const char_t *description,
    const char_t *duration,
    const uint64_t task_id,
    const uint64_t project_id,
    const char_t *project_guid) {

    logger().debug("toggl_start");

    std::string desc("");
    if (description) {
        desc = to_string(description);
    }

    std::string dur("");
    if (duration) {
        dur = to_string(duration);
    }

    std::string p_guid("");
    if (project_guid) {
        p_guid = to_string(project_guid);
    }

    toggl::TimeEntry *te = app(context)->Start(desc, dur,
                           task_id,
                           project_id, p_guid);
    if (te) {
        return copy_string(te->GUID());
    }
    return nullptr;
}

bool_t toggl_continue(
    void *context,
    const char_t *guid) {

    poco_check_ptr(guid);

    std::stringstream ss;
    ss << "toggl_continue guid=" << guid;
    logger().debug(ss.str());

    return toggl::noError == app(context)->Continue(to_string(guid));
}

void toggl_view_time_entry_list(void *context) {
    app(context)->DisplayTimeEntryList(true);
}

void toggl_edit(
    void *context,
    const char_t *guid,
    const bool_t edit_running_entry,
    const char_t *focused_field_name) {

    poco_check_ptr(guid);
    poco_check_ptr(focused_field_name);

    std::stringstream ss;
    ss << "toggl_edit guid=" << guid
       << ", edit_running_entry = " << edit_running_entry
       << ", focused_field_name = " << focused_field_name;
    logger().debug(ss.str());

    app(context)->Edit(to_string(guid),
                       edit_running_entry,
                       to_string(focused_field_name));
}

void toggl_edit_preferences(void *context) {
    app(context)->DisplaySettings(true);
}

bool_t toggl_continue_latest(
    void *context) {

    logger().debug("toggl_continue_latest");

    return toggl::noError == app(context)->ContinueLatest();
}

bool_t toggl_delete_time_entry(
    void *context,
    const char_t *guid) {

    poco_check_ptr(guid);

    std::stringstream ss;
    ss << "toggl_delete_time_entry guid=" << guid;
    logger().debug(ss.str());

    return toggl::noError == app(context)->
           DeleteTimeEntryByGUID(to_string(guid));
}

bool_t toggl_set_time_entry_duration(
    void *context,
    const char_t *guid,
    const char_t *value) {

    poco_check_ptr(guid);
    poco_check_ptr(value);

    std::stringstream ss;
    ss  << "toggl_set_time_entry_duration guid=" << guid
        << ", value=" << value;
    logger().debug(ss.str());

    return toggl::noError == app(context)->SetTimeEntryDuration(
        to_string(guid),
        to_string(value));
}

bool_t toggl_set_time_entry_project(
    void *context,
    const char_t *guid,
    const uint64_t task_id,
    const uint64_t project_id,
    const char_t *project_guid) {

    poco_check_ptr(guid);
    std::string pguid("");
    if (project_guid) {
        pguid = to_string(project_guid);
    }
    return toggl::noError == app(context)->SetTimeEntryProject(to_string(guid),
            task_id,
            project_id,
            pguid);
}

bool_t toggl_set_time_entry_date(
    void *context,
    const char_t *guid,
    const int64_t unix_timestamp) {

    poco_check_ptr(guid);

    std::stringstream ss;
    ss  << "toggl_set_time_entry_date guid=" << guid
        << ", unix_timestamp=" << unix_timestamp;
    logger().debug(ss.str());

    return toggl::noError == app(context)->
           SetTimeEntryDate(to_string(guid), unix_timestamp);
}

bool_t toggl_set_time_entry_start(
    void *context,
    const char_t *guid,
    const char_t *value) {
    poco_check_ptr(guid);
    poco_check_ptr(value);

    std::stringstream ss;
    ss  << "toggl_set_time_entry_start guid=" << guid
        << ", value=" << value;
    logger().debug(ss.str());

    return toggl::noError == app(context)->
           SetTimeEntryStart(to_string(guid), to_string(value));
}

bool_t toggl_set_time_entry_end(
    void *context,
    const char_t *guid,
    const char_t *value) {
    poco_check_ptr(guid);
    poco_check_ptr(value);

    std::stringstream ss;
    ss  << "toggl_set_time_entry_end guid=" << guid
        << ", value=" << value;
    logger().debug(ss.str());

    return toggl::noError == app(context)->
           SetTimeEntryStop(to_string(guid), to_string(value));
}

bool_t toggl_set_time_entry_tags(
    void *context,
    const char_t *guid,
    const char_t *value) {

    poco_check_ptr(guid);
    poco_check_ptr(value);

    std::stringstream ss;
    ss  << "toggl_set_time_entry_tags guid=" << guid
        << ", value=" << value;
    logger().debug(ss.str());

    return toggl::noError == app(context)->SetTimeEntryTags(to_string(guid),
            to_string(value));
}

bool_t toggl_set_time_entry_billable(
    void *context,
    const char_t *guid,
    const bool_t value) {

    poco_check_ptr(guid);

    std::stringstream ss;
    ss  << "toggl_set_time_entry_billable guid=" << guid
        << ", value=" << value;
    logger().debug(ss.str());

    return toggl::noError == app(context)->
           SetTimeEntryBillable(to_string(guid), value);
}

bool_t toggl_set_time_entry_description(
    void *context,
    const char_t *guid,
    const char_t *value) {
    poco_check_ptr(guid);
    poco_check_ptr(value);

    std::stringstream ss;
    ss  << "toggl_set_time_entry_description guid=" << guid
        << ", value=" << value;
    logger().debug(ss.str());

    return toggl::noError == app(context)->
           SetTimeEntryDescription(to_string(guid), to_string(value));
}

bool_t toggl_stop(
    void *context) {
    logger().debug("toggl_stop");

    return toggl::noError == app(context)->Stop();
}

bool_t toggl_discard_time_at(
    void *context,
    const char_t *guid,
    const uint64_t at,
    const bool_t split_into_new_entry) {

    if (!guid) {
        logger().error("Cannot discard time without GUID");
        return false;
    }

    if (!at) {
        logger().error("Cannot discard time without a timestamp");
        return false;
    }

    logger().debug("toggl_discard_time_at");

    return toggl::noError == app(context)->DiscardTimeAt(
        to_string(guid),
        at,
        split_into_new_entry);
}

bool_t toggl_discard_time_and_continue(
    void *context,
    const char_t *guid,
    const uint64_t at) {

    if (!guid) {
        logger().error("Cannot discard time without GUID");
        return false;
    }

    if (!at) {
        logger().error("Cannot discard time without a timestamp");
        return false;
    }

    logger().debug("toggl_discard_time_and_continue");

    return toggl::noError == app(context)->DiscardTimeAndContinue(
        to_string(guid),
        at);
}

bool_t toggl_timeline_toggle_recording(
    void *context,
    const bool_t record_timeline) {
    logger().debug("toggl_timeline_toggle_recording");
    return toggl::noError == app(context)->
           ToggleTimelineRecording(record_timeline);
}

bool_t toggl_timeline_is_recording_enabled(
    void *context) {
    return app(context)->IsTimelineRecordingEnabled();
}

bool_t toggl_feedback_send(
    void *context,
    const char_t *topic,
    const char_t *details,
    const char_t *filename) {
    std::stringstream ss;
    ss << "toggl_feedback_send topic=" << topic << " details=" << details;
    logger().debug(ss.str());

    toggl::Feedback feedback;
    feedback.SetSubject(to_string(topic));
    feedback.SetDetails(to_string(details));
    feedback.SetAttachmentPath(to_string(filename));

    return toggl::noError == app(context)->SendFeedback(feedback);
}

bool_t toggl_set_update_channel(
    void *context,
    const char_t *update_channel) {

    poco_check_ptr(update_channel);

    return toggl::noError == app(context)->
           SetUpdateChannel(to_string(update_channel));
}

char_t *toggl_get_update_channel(
    void *context) {

    std::string update_channel("");
    app(context)->UpdateChannel(&update_channel);
    return copy_string(update_channel);
}

char_t *toggl_get_user_fullname(
    void *context) {

    std::string fullname = app(context)->UserFullName();
    return copy_string(fullname);
}

char_t *toggl_get_user_email(
    void *context) {

    std::string email = app(context)->UserEmail();
    return copy_string(email);
}

int64_t toggl_parse_duration_string_into_seconds(
    const char_t *duration_string) {
    if (!duration_string) {
        return 0;
    }
    return toggl::Formatter::ParseDurationString(to_string(duration_string));
}

void toggl_on_show_app(
    void *context,
    TogglDisplayApp cb) {

    app(context)->UI()->OnDisplayApp(cb);
}

void toggl_on_sync_state(
    void *context,
    TogglDisplaySyncState cb) {

    app(context)->UI()->OnDisplaySyncState(cb);
}

void toggl_on_unsynced_items(
    void *context,
    TogglDisplayUnsyncedItems cb) {

    app(context)->UI()->OnDisplayUnsyncedItems(cb);
}

void toggl_on_error(
    void *context,
    TogglDisplayError cb) {

    app(context)->UI()->OnDisplayError(cb);
}

void toggl_on_online_state(
    void *context,
    TogglDisplayOnlineState cb) {

    app(context)->UI()->OnDisplayOnlineState(cb);
}

void toggl_on_update(
    void *context,
    TogglDisplayUpdate cb) {

    app(context)->UI()->OnDisplayUpdate(cb);
}

void toggl_on_url(
    void *context,
    TogglDisplayURL cb) {

    app(context)->UI()->OnDisplayURL(cb);
}

void toggl_on_login(
    void *context,
    TogglDisplayLogin cb) {

    app(context)->UI()->OnDisplayLogin(cb);
}

void toggl_on_reminder(
    void *context,
    TogglDisplayReminder cb) {

    app(context)->UI()->OnDisplayReminder(cb);
}

void toggl_on_autotracker_notification(
    void *context,
    TogglDisplayAutotrackerNotification cb) {

    app(context)->UI()->OnDisplayAutotrackerNotification(cb);
}

void toggl_on_promotion(
    void *context,
    TogglDisplayPromotion cb) {
    app(context)->UI()->OnDisplayPromotion(cb);
}

void toggl_set_sleep(void *context) {
    app(context)->SetSleep();
}

void toggl_set_wake(void *context) {
    if (!context) {
        return;
    }
    app(context)->SetWake();
}

void toggl_set_online(void *context) {
    if (!context) {
        return;
    }
    app(context)->SetOnline();
}

void toggl_open_in_browser(void *context) {
    app(context)->OpenReportsInBrowser();
}

void toggl_get_support(void *context) {
    app(context)->UI()->DisplayURL(kSupportURL);
}

void toggl_on_workspace_select(
    void *context,
    TogglDisplayViewItems cb) {
    app(context)->UI()->OnDisplayWorkspaceSelect(cb);
}

void toggl_on_client_select(
    void *context,
    TogglDisplayViewItems cb) {
    app(context)->UI()->OnDisplayClientSelect(cb);
}

void toggl_on_tags(
    void *context,
    TogglDisplayViewItems cb) {
    app(context)->UI()->OnDisplayTags(cb);
}

void toggl_on_time_entry_list(
    void *context,
    TogglDisplayTimeEntryList cb) {
    app(context)->UI()->OnDisplayTimeEntryList(cb);
}

void toggl_on_mini_timer_autocomplete(
    void *context,
    TogglDisplayAutocomplete cb) {
    app(context)->UI()->OnDisplayMinitimerAutocomplete(cb);
}

void toggl_on_time_entry_autocomplete(
    void *context,
    TogglDisplayAutocomplete cb) {
    app(context)->UI()->OnDisplayTimeEntryAutocomplete(cb);
}

void toggl_on_project_autocomplete(
    void *context,
    TogglDisplayAutocomplete cb) {
    app(context)->UI()->OnDisplayProjectAutocomplete(cb);
}

void toggl_on_time_entry_editor(
    void *context,
    TogglDisplayTimeEntryEditor cb) {
    app(context)->UI()->OnDisplayTimeEntryEditor(cb);
}

void toggl_on_settings(
    void *context,
    TogglDisplaySettings cb) {
    app(context)->UI()->OnDisplaySettings(cb);
}

void toggl_on_timer_state(
    void *context,
    TogglDisplayTimerState cb) {
    app(context)->UI()->OnDisplayTimerState(cb);
}

void toggl_on_idle_notification(
    void *context,
    TogglDisplayIdleNotification cb) {
    app(context)->UI()->OnDisplayIdleNotification(cb);
}

void toggl_on_autotracker_rules(
    void *context,
    TogglDisplayAutotrackerRules cb) {
    app(context)->UI()->OnDisplayAutotrackerRules(cb);
}

void toggl_debug(const char_t *text) {
    logger().debug(to_string(text));
}

bool_t toggl_check_view_struct_size(
    const int time_entry_view_item_size,
    const int autocomplete_view_item_size,
    const int view_item_size,
    const int settings_size,
    const int autotracker_view_item_size) {
    int size = sizeof(TogglTimeEntryView);
    if (time_entry_view_item_size != size) {
        logger().error("Invalid time entry view item struct size");
        return false;
    }
    size = sizeof(TogglAutocompleteView);
    if (autocomplete_view_item_size != size) {
        logger().error("Invalid autocomplete view item struct size");
        return false;
    }
    size = sizeof(TogglGenericView);
    if (view_item_size != size) {
        logger().error("Invalid view item struct size");
        return false;
    }
    size = sizeof(TogglSettingsView);
    if (settings_size != size) {
        logger().error("Invalid settings view item struct size");
        return false;
    }
    size = sizeof(TogglAutocompleteView);
    if (autocomplete_view_item_size != size) {
        logger().error("Invalid autocomplete view item struct size");
        return false;
    }
    return true;
}

void toggl_set_idle_seconds(
    void *context,
    const uint64_t idle_seconds) {
    if (context) {
        app(context)->SetIdleSeconds(idle_seconds);
    }
}

bool_t toggl_set_promotion_response(
    void *context,
    const int64_t promotion_type,
    const int64_t promotion_response) {
    return toggl::noError == app(context)->SetPromotionResponse(
        promotion_type, promotion_response);
}

char_t *toggl_run_script(
    void *context,
    const char* script,
    int64_t *err) {

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    toggl_register_lua(context, L);
    lua_settop(L, 0);

    *err = luaL_loadstring(L, script);
    if (*err) {
        return copy_string(lua_tostring(L, -1));
    }

    *err = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (*err) {
        return copy_string(lua_tostring(L, -1));
    }

    int argc = lua_gettop(L);

    std::stringstream ss;
    ss << argc << " value(s) returned" << std::endl;

    for (int i = 0; i < argc; i++) {
        if (lua_isstring(L, -1)) {
            ss << lua_tostring(L, -1);
        } else if (lua_isnumber(L, -1)) {
            ss << lua_tointeger(L, -1);
        } else if (lua_isboolean(L, -1)) {
            ss << lua_toboolean(L, -1);
        } else {
            ss << "ok";
        }
        lua_pop(L, -1);
    }
    ss << std::endl << std::endl;

    lua_close(L);

    return copy_string(ss.str());
}

bool_t toggl_autotracker_add_rule(
    void *context,
    const char_t *term,
    const uint64_t project_id) {

    poco_check_ptr(term);

    return toggl::noError == app(context)->
           AddAutotrackerRule(to_string(term), project_id);
}

bool_t toggl_autotracker_delete_rule(
    void *context,
    const int64_t id) {

    return toggl::noError == app(context)->DeleteAutotrackerRule(id);
}

void testing_sleep(
    const int seconds) {
    Poco::Thread::sleep(seconds*1000);
}

bool_t testing_set_logged_in_user(
    void *context,
    const char *json) {
    poco_check_ptr(json);

    toggl::Context *ctx = reinterpret_cast<toggl::Context *>(context);
    return toggl::noError == ctx->SetLoggedInUserFromJSON(std::string(json));
}
