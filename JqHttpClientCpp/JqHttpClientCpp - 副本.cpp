// JqHttpClientCpp.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <string>
#include <cpprest/http_client.h>

using namespace std;

namespace  jq_apis
{
	const string url = "https://dataapi.joinquant.com/apis";

	namespace  client
	{
		using namespace web;                        // Common features like URIs.
		using namespace web::http;                  // Common HTTP functionality
		using namespace web::http::client;          // HTTP client features

		class CJoinQuantCPP
		{
			public:
				CJoinQuantCPP() :m_client(L"https://dataapi.joinquant.com/apis2") {} ;
				~CJoinQuantCPP() {};

				string get_token(std::string url, std::string mob, std::string pwd)
				{
					json::value json_query;
					json_query[L"method"] = json::value::string(U("get_token"));
					json_query[L"mob"] = json::value::string(utility::conversions::to_string_t(mob));
					json_query[L"pwd"] = json::value::string(utility::conversions::to_string_t(pwd));

					
					m_client = http_client(utility::conversions::to_string_t(url));

					wcout <<"Debug:  "<< utility::conversions::to_string_t(url).c_str() << endl;

					//auto request = _make_request(methods::POST, utility::string_t(U("/")), json_query);
					auto request = _make_request2(methods::POST, utility::conversions::to_string_t(url), json_query);
					return request.then([](http_response response)
					{

						cout << "Server returned returned status code " << response.status_code() <<std::endl;
						if (response.status_code() == status_codes::OK)
						{
							std::wostringstream stream;
							stream << L"Server returned returned status code " << response.status_code() << L'.' << std::endl;
							std::wcout << stream.str();

							stream.str(std::wstring());
							stream << L"Content type: " << response.headers().content_type() << std::endl;
							stream << L"Content length: " << response.headers().content_length() << L"bytes" << std::endl;
							std::wcout << stream.str();

							response.content_ready().wait();
							return response.extract_utf8string().get();
						}
					}).get();
				}

				string get_price(std::string url, std::string token, std::string security)
				{

					json::value json_query;
					json_query[L"method"] = json::value::string(U("get_security_info"));
					json_query[L"token"] = json::value::string(utility::conversions::to_string_t(token));
					json_query[L"code"] = json::value::string(utility::conversions::to_string_t(security));

					auto response = _make_request(methods::POST, utility::string_t(U("/")), json_query);
					return response.then([](http_response response)
					{
						if (response.status_code() == status_codes::OK)
						{
							response.content_ready().wait();
							return response.extract_utf8string().get();
						}
					}).get();
				}

				static void save_csv(string info, string filename)
				{
					ofstream outFile;
					outFile.open(filename, ios::out);
					outFile << info;
					outFile.close();
				}

				static string  read_csv(string filename)
				{
					ifstream inFile(filename);
					ostringstream info;

					string line;
					while (getline(inFile, line)) {
						info << line << endl;
					}
					return info.str();
				}


			private:
				web::http::client::http_client m_client;
				utility::string_t m_token;

				pplx::task<web::http::http_response> _make_request(web::http::method method,
					const utility::string_t& path,
					const web::json::value& data)
				{
					web::http::uri_builder bldr;
					bldr.append(path);

					web::http::http_request req(method);
					req.set_request_uri(bldr.to_string());
					req.set_body(data);
					return _make_request(req);
				}


				pplx::task<web::http::http_response> _make_request2(web::http::method method,
					const utility::string_t& address2,
					const web::json::value& data)
				{
					utility::string_t address = U("https://dataapi.joinquant.com/apis");
					http::uri uri = http::uri(address);
					web::http::uri_builder bldr(uri);
					bldr.append_path(U("/"));

					web::http::http_request req(method);
					req.set_request_uri(bldr.to_string());
					req.set_body(data);
					return _make_request(req);
				}

				pplx::task<web::http::http_response>  _make_request(web::http::http_request req)
				{
					return m_client.request(req);
				}

		};

	}//namespace client

} //namespace jq_apis

using namespace jq_apis::client;

int main()
{
	const std::string url = "https://dataapi.joinquant.com/apis";
	const std::string mob = "19925473023";
	const std::string pwd = "KoKo150118";

	CJoinQuantCPP client;
	// 获取 token
	string token = client.get_token(url, mob, pwd);
	cout << "token:\n" << token << endl;

	//// 获取标的的价格信息
	//string security_info = client.get_price(url, token, string("502050.XSHG"));
	//cout << "security info: \n" << security_info << endl;

	////写文件
	//string file_name = "security_info.csv";
	//client.save_csv(security_info, file_name);

	////读文件
	//string text = client.read_csv(file_name);
	//cout << "In csv file: \n" << text;
	return 0;

}

