#include "stdafx.h"
#include <string>
#include <cpprest/http_client.h>

using namespace std;

namespace  jq_cpp_client
{
	using namespace web;                        // Common features like URIs.
	using namespace web::http;                  // Common HTTP functionality
	using namespace web::http::client;          // HTTP client features

	class CJoinQuantCPP
	{
	public:
		CJoinQuantCPP(string url) :m_client(utility::conversions::to_utf16string(url)) {};
		CJoinQuantCPP() : m_client(L"https://dataapi.joinquant.com/apis") {};
		~CJoinQuantCPP() {};

		string get_token(std::string url, std::string mob, std::string pwd)
		{

			json::value json_query;
			json_query[L"method"] = json::value::string(U("get_token"));
			json_query[L"mob"] = json::value::string(utility::conversions::to_string_t(mob));
			json_query[L"pwd"] = json::value::string(utility::conversions::to_string_t(pwd));

			m_client = http_client(utility::conversions::to_utf16string(url));
			utility::string_t path = utility::string_t(U("/"));

			auto rsp = _make_request(methods::POST, path, json_query);
			return rsp.then([&](http_response rsp) {
				m_token = _check_response(rsp);
				return m_token;
			}).get();

		}

		string get_price(std::string code)
		{
			json::value json_query;
			json_query[L"method"] = json::value::string(U("get_security_info"));
			json_query[L"token"] = json::value::string(utility::conversions::to_string_t(m_token));
			json_query[L"code"] = json::value::string(utility::conversions::to_string_t(code));

			utility::string_t path = utility::string_t(U("/"));

			auto rsp = _make_request(methods::POST, path, json_query);
			return rsp.then([&](http_response rsp) {
				return _check_response(rsp);
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
		string m_token;

		pplx::task<web::http::http_response>  _make_request(web::http::method method,
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

		pplx::task<web::http::http_response>  _make_request(web::http::http_request req)
		{
			return m_client.request(req);
		}

		string _check_response(web::http::http_response rsp) {
			if (rsp.status_code() == status_codes::ServiceUnavailable)
			{
				return string("ServiceUnavailable");
			}

			if (rsp.status_code() == status_codes::NotImplemented)
			{
				return string("Method Not Implemented");
			}

			if (rsp.status_code() == status_codes::OK)
			{
				//rsp.content_ready().wait();
				return rsp.extract_utf8string().get();
			}
			return string("Unknow Error");
		}
	};
}//namespace jq_cpp_client

using namespace jq_cpp_client;

int main()
{
	const std::string url = "https://dataapi.joinquant.com/apis";
	const std::string mob = "XXXXXX";
	const std::string pwd = "XXXXXX";

	CJoinQuantCPP client(url);
	// 获取 token
	string token = client.get_token(url, mob, pwd);
	cout << token << endl;

	// 获取标的的价格信息
	string security_info = client.get_price(string("502050.XSHG"));
	cout << security_info << endl;

	//写文件
	string file_name = "security_info.csv";
	client.save_csv(security_info, file_name);

	//读文件
	string text = client.read_csv(file_name);
	cout << text;

	return 0;
}