#define DISCORDPP_IMPLEMENTATION
#include "discordpp.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <functional>
#include <csignal>

// Replace with your Discord Application ID
const uint64_t APPLICATION_ID = 1397338041604902983;
std::atomic<bool> running = true;

// Function to set up callbacks for the Discord client
// This will handle logging and status changes
void setupCallbacks(std::shared_ptr<discordpp::Client> client) {
    client->AddLogCallback([](std::string message, discordpp::LoggingSeverity severity) {
        std::cout << "[" << EnumToString(severity) << "] " << message << std::endl;
    }, discordpp::LoggingSeverity::Info);

    client->SetStatusChangedCallback([client](discordpp::Client::Status status, discordpp::Client::Error error, int32_t errorDetail) {
      std::cout << "🔄 Status changed: " << discordpp::Client::StatusToString(status) << std::endl;

      if (status == discordpp::Client::Status::Ready) {

        // Access initial relationships data
        std::cout << "👥 Friends Count: " << client->GetRelationships().size() << std::endl;

        // Configure rich presence details
        discordpp::Activity activity;
        activity.SetType(discordpp::ActivityTypes::Playing);
        activity.SetState("Running with Whoop 4.0");
        activity.SetDetails("Current Strain: 5.0");

        discordpp::ActivityAssets assets;
        assets.SetLargeImage("black_logo");
        assets.SetLargeText("Running");
        assets.SetSmallImage("running");

        activity.SetAssets(assets);

        // Update rich presence
        client->UpdateRichPresence(activity, [](discordpp::ClientResult result) {
            if(result.Successful()) {
                std::cout << "🎮 Rich Presence updated successfully!\n";
            } else {
                std::cerr << "❌ Rich Presence update failed";
            }
        });


    } else if (error != discordpp::Client::Error::None) {
        std::cerr << "❌ Connection Error: " << discordpp::Client::ErrorToString(error) << " - Details: " << errorDetail << std::endl;
      }
    });

}

// Signal handler to stop the application
void signalHandler(int signum) {
    std::cout << "\n🔴 Stopping Discord SDK...\n";
    running.store(false);
}

int main() {
    // This will catch CTRL + C and other termination signals
    std::signal(SIGINT, signalHandler);

    std::shared_ptr<discordpp::Client> client = std::make_shared<discordpp::Client>();
    setupCallbacks(client);

    // Generate OAuth2 code verifier for authentication
    auto codeVerifier = client->CreateAuthorizationCodeVerifier();

    // Set up authentication arguments
    discordpp::AuthorizationArgs args{};
    args.SetClientId(APPLICATION_ID);
    args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
    args.SetCodeChallenge(codeVerifier.Challenge());

    // Begin authentication process
    client->Authorize(args, [client, codeVerifier](auto result, auto code, auto redirectUri) {
      if (!result.Successful()) {
        std::cerr << "❌ Authentication Error: " << result.Error() << std::endl;
        return;
      } else {
        std::cout << "✅ Authorization successful! Getting access token...\n";

        // Exchange auth code for access token
        client->GetToken(APPLICATION_ID, code, codeVerifier.Verifier(), redirectUri,
          [client](discordpp::ClientResult result,
          std::string accessToken,
          std::string refreshToken,
          discordpp::AuthorizationTokenType tokenType,
          int32_t expiresIn,
          std::string scope) {
            std::cout << "🔓 Access token received! Establishing connection...\n";
            // Next Step: Update the token and connect
            client->UpdateToken(discordpp::AuthorizationTokenType::Bearer,  accessToken, [client](discordpp::ClientResult result) {
              if(result.Successful()) {
                std::cout << "🔑 Token updated, connecting to Discord...\n";
                client->Connect();
              }
            });
        });
      }
    });

    // Keep application running to allow SDK to receive events and callbacks
    while (running) {
      discordpp::RunCallbacks();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
