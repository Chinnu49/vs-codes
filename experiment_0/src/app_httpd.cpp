#include "esp_http_server.h"
#include "esp_camera.h"
#include "Arduino.h"

httpd_handle_t server = NULL;

// 📡 STREAM HANDLER
static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t * fb = NULL;

    httpd_resp_set_type(req,
        "multipart/x-mixed-replace;boundary=frame");

    while (true)
    {
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            return ESP_FAIL;
        } 

        httpd_resp_send_chunk(req, "--frame\r\n", 8);
        httpd_resp_send_chunk(req,
            "Content-Type: image/jpeg\r\n\r\n", 28);
        httpd_resp_send_chunk(req,
            (const char *)fb->buf, fb->len);
        httpd_resp_send_chunk(req, "\r\n", 2);

        esp_camera_fb_return(fb);
    }

    return ESP_OK;
}

// 🚀 START SERVER
void startCameraServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_start(&server, &config);

    httpd_uri_t uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL
    };

    httpd_register_uri_handler(server, &uri);

    Serial.println("Camera Web Server Started");
}