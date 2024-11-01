#include <iostream>
#include <string>
#include <cstdio>
#include <csignal>
#include <atomic>
#include <unistd.h>
#include <sys/select.h>
#include <cstdlib>
#include <regex> 
#include <curl/curl.h>

struct upload_status {
    const char *payload;
    size_t bytes_left;
};

size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    upload_status *upload_ctx = static_cast<upload_status *>(userp);
    if (upload_ctx->bytes_left == 0) {
        return 0; // No more data to send
    }

    size_t len = upload_ctx->bytes_left;
    if (len > size * nmemb) {
        len = size * nmemb;
    }

    std::memcpy(ptr, upload_ctx->payload, len);
    upload_ctx->payload += len;
    upload_ctx->bytes_left -= len;
    return len;
}

void mailer(const std::string& email) {
    CURL *curl;
    CURLcode res = CURLE_OK;

    // Initialize libcurl
    curl = curl_easy_init();
    if(curl) {
        // Set the SMTP server URL (using Gmail's SMTP with SSL/TLS)
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");

        // Set the username and password for Gmail authentication
        curl_easy_setopt(curl, CURLOPT_USERNAME, "brucewaynebatmangotham92@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "pvodpxlesymelkav");

        // Set the "From" email address
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<brucewaynebathmangotham92@gmail.com>");

        // Set the "To" email address
        struct curl_slist *recipients = nullptr;
        recipients = curl_slist_append(recipients, ("<" + email + ">").c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Define the email message
        const std::string payload_text = 
            "To: " + email + "\r\n"
            "From: brucewaynebathmangotham92@gmail.com\r\n"
            "Subject: Test Email\r\n"
            "\r\n"
            "This is a test email sent from C++ using libcurl.\r\n";

        upload_status upload_ctx = { payload_text.c_str(), payload_text.size() };

        // Configure payload callback
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Enable SSL/TLS
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        // Perform the send
        res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Email sent successfully!" << std::endl;
        }

        // Clean up
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
}

void emailvalidation(const std::string& email) {
    std::string newemail;
    const std::regex pattern ("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
    if (std::regex_match(email,pattern)) {
        std::cout << "Please wait and check your email for next steps." << std::endl;
        mailer(email);
    } else {
        std::cout << "Invalid email please try again: ";
        std::cin >> newemail;
        std::cout << std::endl;
        emailvalidation(newemail);
    }
}

int main() {
    std::string email;
    std::cout << "Please enter a valid email address: ";
    std::cin >> email;
    emailvalidation(email);
    std::cout << std::endl;
}