#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <sstream>

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

void mailer(const std::string& email, const std::string& image_path) {
    CURL *curl;
    CURLcode res = CURLE_OK;

    curl = curl_easy_init();
    if (curl) {
        // Set SMTP server URL
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");

        // Set authentication
        curl_easy_setopt(curl, CURLOPT_USERNAME, "brucewaynebatmangotham92@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "pvodpxlesymelkav");

        // Set "From" and "To" email addresses
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<brucewaynebatmangotham92@gmail.com>");
        struct curl_slist *recipients = nullptr;
        recipients = curl_slist_append(recipients, ("<" + email + ">").c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Create MIME structure for email
        curl_mime *mime;
        curl_mimepart *part;
        mime = curl_mime_init(curl);

        // Add the email text part
        part = curl_mime_addpart(mime);
        curl_mime_data(part, "This is a test email sent from C++ with an image attachment.", CURL_ZERO_TERMINATED);

        // Add the subject header
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "Subject: Test Email with Attachment\r\n");

        // Add the image attachment part
        part = curl_mime_addpart(mime);
        curl_mime_filedata(part, image_path.c_str());
        curl_mime_filename(part, "image.jpg");  // Name of the attachment
        curl_mime_type(part, "image/jpeg");     // MIME type

        // Attach the MIME structure to the email
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        // Enable SSL/TLS
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        // Send the email
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Email with image sent successfully!" << std::endl;
        }

        // Clean up
        curl_slist_free_all(recipients);
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
    }
}

void emailvalidation(const std::string& email, const std::string& image_path) {
    std::string newemail;
    const std::regex pattern ("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
    if (std::regex_match(email,pattern)) {
        std::cout << "Please wait and check your email for next steps." << std::endl;
        mailer(email, image_path);
    } else {
        std::cout << "Invalid email please try again: ";
        std::cin >> newemail;
        std::cout << std::endl;
        emailvalidation(newemail, image_path);
    }
}

int main() {
    std::string email;
    std::string image_path = "path/to/your/image.jpg";
    std::cout << "Please enter a valid "
    email_validation(mail, image_path);
    return 0;
}