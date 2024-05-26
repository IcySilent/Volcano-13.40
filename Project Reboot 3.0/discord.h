#pragma once

#define CURL_STATICLIB

#include <iostream>
#include <string>
#include <curl/curl.h>
#include "globals.h"

inline std::string iso8601() {
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    tm* t = new tm();
    gmtime_s(t, &now);
    strftime(buf, sizeof buf, "%FT%TZ", t);
    return std::string(buf);
}

class DiscordWebhook {
public:
    // Parameters:
    // - webhook_url: the discord HostingWebHook url
    DiscordWebhook(const char* webhook_url)
    {
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, webhook_url);

            // Discord webhooks accept json, so we set the content-type to json data.
            curl_slist* headers = curl_slist_append(NULL, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
        else {
            std::cerr << "Error: curl_easy_init() returned NULL pointer" << '\n';
        }
    }

    ~DiscordWebhook()
    {
        curl_global_cleanup();
        curl_easy_cleanup(curl);
    }

    bool handleCode(CURLcode res)
    {
        return res == CURLE_OK;
    }

    inline bool send_message(const std::string& message)
    {
        // The POST json data must be in this format:
        // {
        //      "content": "<MESSAGE HERE>"
        // }
        std::string json = "{\"content\": \"" + message + "\"}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

        bool success = handleCode(curl_easy_perform(curl));

        return success;
    }
    inline bool send_embedjson(const std::string ajson)
    {
        std::string json = ajson.contains("embeds") ? ajson : "{\"embeds\": " + ajson + "}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

        bool success = handleCode(curl_easy_perform(curl));

        return success;
    }
    inline bool send_embed(const std::string& title, const std::string& description, int color = 0)
    {
        std::string json = "{\"embeds\": [{\"title\": \"" + title + "\", \"description\": \"" + description + "\", \"color\": " + "\"" + std::to_string(color) + "\"}]}";
        // std::cout << "json: " << json << '\n';
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

        bool success = handleCode(curl_easy_perform(curl));

        return success;
    }
    inline bool send_embed_with_ping(const std::string& title, const std::string& description, int color = 0)
    {
        std::string json = "{\"content\": \"<@&" + Globals::RolePing + ">\", \"embeds\": [{\"title\": \"" + title + "\", \"description\": \"" + description + "\", \"color\": " + "\"" + std::to_string(color) + "\"}]}";
        // std::cout << "json: " << json << '\n';
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

        bool success = handleCode(curl_easy_perform(curl));

        return success;
    }

    inline bool send_status(const std::string& version, std::string& playlist, int color = 0)
    {
        std::string json = "{\"embeds\": [{\"title\": \"Server up!\", \"fields\": [{\"name\":\"Version\",\"value\":\"" + version + "\"},{\"name\":\"Playlist\",\"value\":\"" + playlist + "\"}], \"color\": " + "\"" + std::to_string(color) + "\", \"footer\": {\"text\":\"    \", \"icon_url\":\"https://media.discordapp.net/attachments/1190820081639358464/1193388772142878730/ac54ee68ce1ab734b435abec0b5f9a60.png\"}, \"timestamp\":\"" + iso8601() + "\"}] }";
        // std::cout << "json: " << json << '\n';
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

        bool success = curl_easy_perform(curl) == CURLE_OK;

        return success;
    }

    inline bool send_started(const std::string& version, std::string& players, int color = 0)
    {
        std::string json = "{\"embeds\": [{\"title\": \"Match started!\", \"fields\": [{\"name\":\"Version\",\"value\":\"" + version + "\"},{\"name\":\"Players\",\"value\":\"" + players + "\"}], \"color\": " + "\"" + std::to_string(color) + "\", \"footer\": {\"text\":\"Flipped\", \"icon_url\":\"https://media.discordapp.net/attachments/1190820081639358464/1193388772142878730/ac54ee68ce1ab734b435abec0b5f9a60.png\"}, \"timestamp\":\"" + iso8601() + "\"}] }";
        // std::cout << "json: " << json << '\n';
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

        bool success = curl_easy_perform(curl) == CURLE_OK;

        return success;
    }

    inline bool send_restarting(const std::string& version, int color = 0)
    {
        std::string json = "{\"embeds\": [{\"title\": \"Server restarting...\", \"fields\": [{\"name\":\"Version\",\"value\":\"" + version + "\"}], \"color\": " + "\"" + std::to_string(color) + "\", \"footer\": {\"text\":\"flipped\", \"icon_url\":\"https://media.discordapp.net/attachments/1190820081639358464/1193388772142878730/ac54ee68ce1ab734b435abec0b5f9a60.png\"}, \"timestamp\":\"" + iso8601() + "\"}] }";
        // std::cout << "json: " << json << '\n';
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

        bool success = curl_easy_perform(curl) == CURLE_OK;

        return success;
    }

private:
    CURL* curl;
};

namespace Information
{
    static std::string UptimeWebHook = (Globals::Webhook);
}

static DiscordWebhook UptimeWebHook(Information::UptimeWebHook.c_str());