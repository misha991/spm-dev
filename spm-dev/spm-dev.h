#include <iostream>  
#include <fstream>
#include <stdio.h>
#include <filesystem>
#include <thread>
#include <chrono>
#include <string>
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdexcept>
#include <string>
#include <math.h>
#include "httplib.h"
#include "indicators.hpp"
namespace spm {

	namespace core {
		class Downloader {
		private:
			static int download_progress_callback(void* clientp, curl_off_t dltotal,
				curl_off_t dlnow, curl_off_t ultotal,
				curl_off_t ulnow)
			{
				indicators::ProgressBar* progress_bar =
					static_cast<indicators::ProgressBar*>(clientp);

				if (progress_bar->is_completed())
				{
					;
				}
				else if (dltotal == 0)
				{
					progress_bar->set_progress(0);
				}
				else
				{
					int percentage =
						static_cast<float>(dlnow) / static_cast<float>(dltotal) * 100;
					progress_bar->set_progress(percentage);
				}

				// If your callback function returns CURL_PROGRESSFUNC_CONTINUE it will
				// cause libcurl to continue executing the default progress function. return
				// CURL_PROGRESSFUNC_CONTINUE;

				return 0;
			}

			static int download_progress_default_callback(void* clientp, curl_off_t dltotal,
				curl_off_t dlnow, curl_off_t ultotal,
				curl_off_t ulnow)
			{
				return CURL_PROGRESSFUNC_CONTINUE;
			}

			std::string extract_file_name(const std::string& url)
			{
				int i = url.size();
				for (; i >= 0; i--)
				{
					if (url[i] == '/')
					{
						break;
					}
				}

				return url.substr(i + 1, url.size() - 1);
			}
		public:
			bool download(const std::string& url, const std::string& file_path)
			{
				const std::string file_name = extract_file_name(url);

				// Hide cursor
				indicators::show_console_cursor(false);

				indicators::ProgressBar progress_bar{
					indicators::option::BarWidth{30}, indicators::option::Start{" ["},
					indicators::option::Fill{"#"}, indicators::option::Lead{"#"},
					indicators::option::Remainder{"-"}, indicators::option::End{"]"},
					indicators::option::PrefixText{file_name},
					// indicators::option::ForegroundColor{indicators::Color::yellow},
					indicators::option::ShowElapsedTime{true},
					indicators::option::ShowRemainingTime{true},
					// indicators::option::FontStyles{
					//     std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
				};

				CURL* curl;
				FILE* fp;
				CURLcode res;
				curl = curl_easy_init();
				if (curl)
				{
					fp = fopen(file_path.c_str(), "wb");
					curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
					curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION,
						download_progress_callback);
					curl_easy_setopt(curl, CURLOPT_XFERINFODATA,
						static_cast<void*>(&progress_bar));
					curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
					// Perform a file transfer synchronously.
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
					fclose(fp);
				}
				indicators::show_console_cursor(true);
				if (res == CURLE_OK)
				{
					std::cout << "Downloaded file " << file_name << std::endl;
					return true;
				}
				else
				{
					std::cout << "Error while downloading: " << res << std::endl;
					return false;
				}
			}

		};
		std::vector<std::string> split(std::string str, char delim)
		{
			std::vector<std::string> result;
			auto left = str.begin();
			for (auto it = left; it != str.end(); ++it)
			{
				if (*it == delim)
				{
					result.emplace_back(&*left, it - left);
					left = it + 1;
				}
			}
			if (left != str.end())
				result.emplace_back(&*left, str.end() - left);
			return result;
		}
		class ProgressBar {
		private:

			float progress = 0.0;

			float max = 1.0;

		public:
			float step;
			int color;
			std::string text;
			ProgressBar(std::string text, int color, float st) {
				this->color = color;
				this->text = text;
				this->step = max / st;
			}
			void tick() {
				indicators::ProgressBar progress_bar{
					indicators::option::BarWidth{30}, indicators::option::Start{" ["},
					indicators::option::Fill{"#"}, indicators::option::Lead{"#"},
					indicators::option::Remainder{"-"}, indicators::option::End{"]"},
					indicators::option::PrefixText{text},
					// indicators::option::ForegroundColor{indicators::Color::yellow},
					indicators::option::ShowElapsedTime{true},
					indicators::option::ShowRemainingTime{true},
					// indicators::option::FontStyles{
					//     std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
				};
				if (progress < max) {
					progress = progress + step;
					progress_bar.set_progress(progress * 100);



				}
				//std::cout << std::endl;
			}
			void reset() {
				std::cout << std::endl;
				progress = 0.0;
			}
		};
		class File {
		private:

		public:
			const char* path;
			File() {

			}
			File(const char* path) {
				this->path = path;
				if (access(path, F_OK) != -1) {
					//#ifdef DEBUG

					//#endif // DEBUG
				}
				else {
					std::ofstream outfile(path);

					outfile << "" << std::endl;

					outfile.close();
					if (access(path, F_OK) != -1) {

					}
					else {
						std::cout << "Error reading file" << std::endl;
						exit(1);
					}

				}
			}
			std::string readAllText() {
				std::ifstream t(path);
				std::string str((std::istreambuf_iterator<char>(t)),
					std::istreambuf_iterator<char>());
				return str;
			}
			int copyTo(std::string copyPath) {
				std::string line;
				// For writing text file 
				// Creating ofstream & ifstream class object 
				std::ifstream ini_file = std::ifstream(path);
				// This is the original file 
				std::ofstream out_file = std::ofstream(copyPath);
				if (ini_file && out_file) {

					while (getline(ini_file, line)) {
						out_file << line << "\n";
					}
				}
				else {
					return 1;
				}
				// Closing file 
				ini_file.close();
				out_file.close();
				return 0;
			}
			int append(std::string text) {
				std::ofstream out;

				// std::ios::app is the open mode "append" meaning
				// new data will be written to the end of the file.
				out.open(path, std::ios::app);

				std::string str = text;
				out << str;
				out.close();
				return 0;
			}
			int appendLine(std::string text) {
				std::ofstream out;

				// std::ios::app is the open mode "append" meaning
				// new data will be written to the end of the file.
				out.open(path, std::ios::app);

				std::string str = text;
				out << str << std::endl;
				out.close();
				return 0;
			}
			int removeLine(std::string rLine) {
				std::string line;
				std::ifstream fin;
				//mirror list path
				fin.open(path);
				// contents of path must be copied to a temp file then
				// renamed back to the path file
				mkdir("/tmp/spm", 0777);
				std::ofstream temp;
				temp.open("/tmp/spm/spm_rline_temp", std::ios::app);
				while (getline(fin, line)) {
					// write all lines to temp other than the line marked for erasing
					if (line != rLine)
						temp << line << std::endl;
				}
				//create temp dir

				temp.close();
				fin.close();
				//remove previos version of mirror list;
				std::remove(path);
				std::ifstream in("/tmp/spm/spm_rline_temp", std::ios::in | std::ios::binary);
				std::ofstream out(path, std::ios::out | std::ios::binary);
				out << in.rdbuf();
				//removing temp file
				std::remove("/tmp/spm/spm_rline_temp");
				in.close();
				out.close();
				return 0;
			}
		};
	}
}

int execute(const char* command) {
	std::cout << "- " << command << std::endl;
	return system(command);
}
int repositoryAddAutoControlPakagePakage(char* pkgBuildPath,char* pakageUrl,char* targetRepoDir) {
	std::string cdCmd = "cd ";
	cdCmd.append(targetRepoDir);
	execute(cdCmd.c_str());
	std::string downloadUrl;
}
int repositoryAddPakage(char* pkgPath, char* targetRepoDir) {

}
int webServerInit() {

}


