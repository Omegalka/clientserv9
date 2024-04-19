#include <iostream>
#include <curl/curl.h>
#include <jpeglib.h>
#include <fstream>

#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "libjpeg.lib")

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* data) {
    data->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void DownloadImage(const string& url, const string& filename) {
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        ofstream ofs(filename, ios::binary);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }

        curl_easy_cleanup(curl);
    }
}

void SaveJPEGImage(const string& filename, const unsigned char* data, int width, int height) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE* outfile;
    if ((outfile = fopen(filename.c_str(), "wb")) == NULL) {
        cerr << "Can't open " << filename << " for writing." << endl;
        return;
    }

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride = width * 3;

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = const_cast<unsigned char*>(&data[cinfo.next_scanline * row_stride]);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);

    jpeg_destroy_compress(&cinfo);
}

int main() {
    curl_global_init(CURL_GLOBAL_ALL);

    while (true) {
        cout << "Press Enter to download a random image (or type 'q' to quit): ";
        string input;
        getline(cin, input);

        if (input == "q" || input == "Q") {
            break;
        }

 
        string imageUrl = "https://example.com/random-image-generator";
        string filename = "image.jpg";
        DownloadImage(imageUrl, filename)
        cout << "Image downloaded and saved as " << filename;
    }

    curl_global_cleanup();
    return 0;
}