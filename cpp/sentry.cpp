#include "sentry.h"
#include <sentry.h>

#include "common/version.h"
#include "map/utils/moduleutils.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>

SentryModule::~SentryModule()
{
    sentry_close();
}

std::string GetSpdlogFilePath()
{
    if (const auto logger = spdlog::get("error"))
    {
        for (const auto& sink : logger->sinks())
        {
            if (const auto fileSink = std::dynamic_pointer_cast<spdlog::sinks::basic_file_sink_mt>(sink))
            {
                return fileSink->filename();
            }
            if (const auto dailySink = std::dynamic_pointer_cast<spdlog::sinks::daily_file_sink_mt>(sink))
            {
                return dailySink->filename();
            }
        }
    }

    return "";
}

void SentryModule::OnInit()
{
    sentry_options_t* options = sentry_options_new();
    sentry_options_set_dsn(options, settings::get<std::string>("main.SENTRY_DSN").c_str());
    sentry_options_set_database_path(options, ".sentry-native");

    const std::string logPath = GetSpdlogFilePath();
    if (!logPath.empty())
    {
        sentry_options_add_attachment(options, logPath.c_str());
    }

    sentry_options_add_attachment(options, "log/connect-server.log");
    sentry_options_add_attachment(options, "log/world-server.log");

    const std::string releaseInfo = std::format("xi_map@{}", version::GetGitSha());
    sentry_options_set_release(options, releaseInfo.c_str());

    sentry_options_set_debug(options, settings::get<bool>("logging.LOG_DEBUG"));
    sentry_init(options);

    sentry_set_tag("git.branch", version::GetGitBranch());
    sentry_set_tag("git.commit", version::GetGitSha());
    sentry_set_tag("git.date", version::GetGitDate());
    sentry_set_tag("version", version::GetVersionString());

    const sentry_value_t buildContext = sentry_value_new_object();
    sentry_value_set_by_key(buildContext, "commit_message", sentry_value_new_string(version::GetGitCommitSubject()));
    sentry_value_set_by_key(buildContext, "git_date", sentry_value_new_string(version::GetGitDate()));
    sentry_value_set_by_key(buildContext, "version_string", sentry_value_new_string(version::GetVersionString()));
    sentry_set_context("build", buildContext);
}

REGISTER_CPP_MODULE(SentryModule);