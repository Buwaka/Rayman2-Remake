#pragma once
#include <fstream>

class TextReader
{
public:
	TextReader();
	~TextReader() = default;

	std::string ReadLine();

	void Open(std::string Textfile);
	bool Exists() const { return m_Exists; };
	bool eof();
	UINT size() { return m_size; };
private:

	bool m_Exists;
	size_t m_size;
	size_t m_Counter;
	std::vector<std::string> m_Lines;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	TextReader(const TextReader& yRef);
	TextReader& operator=(const TextReader& yRef);
};

class TextReaderW
{
public:
	TextReaderW();
	~TextReaderW();

	int ReadLine(std::wstring& out);


	void Open(std::wstring Textfile);
	void Close();
	bool Exists() const { return m_Exists; }
private:

	bool m_Exists;
	std::wifstream* m_pReader;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	TextReaderW(const TextReader& yRef);
	TextReaderW& operator=(const TextReaderW& yRef);
};



inline std::string To_stringSimple(std::wstring ws)
{
	std::string s;
	s.assign(ws.begin(), ws.end());

	return s;
}

inline std::wstring To_wstringSimple(std::string s)
{
	std::wstring ws;
	ws.assign(s.begin(), s.end());

	return ws;
}

inline std::vector<std::string> tokenize(const std::string source, char delimiter)
{
	std::vector<std::string> results;

	size_t prev = 0;
	size_t next = 0;

	while ((next = source.find_first_of(delimiter, prev)) != std::string::npos)
	{
		if ((next - prev != 0))
		{
			results.push_back(source.substr(prev, next - prev));
		}
		prev = next + 1;
	}

	if (prev < source.size())
	{
		results.push_back(source.substr(prev));
	}

	return results;


	//char *next_token;
	//char *token = strtok_s(const_cast<char*>(str.c_str()), &delim, &next_token);
	//while (token != nullptr)
	//{
	//	out.push_back(std::string(token));
	//	token = strtok_s(nullptr, &delim, &next_token);
	//}
	//return out;
}

inline std::wstring ToWUpper(const std::wstring& s)
{
	std::wstring str = s;
	for (size_t i = 0; i < str.size(); i++)
	{
		str[i] = (wchar_t)towupper(str[i]);
	}
	return str;
}

inline std::string ToUpper(const std::string& s)
{
	std::string str = s;
	for (size_t i = 0; i < str.size(); i++)
	{
		str[i] = (char)toupper(str[i]);
	}
	return str;
}

inline std::wstring getFileExt(const std::wstring& s)
{

	size_t i = s.rfind('.', s.length());
	if (i != std::string::npos)
	{
		return(s.substr(i + 1, s.length() - i));
	}

	return(L"");
}

inline std::wstring GetPath(const std::wstring& s)
{

	size_t i = s.rfind('/', s.length());
	if (i != std::string::npos)
	{
		return(s.substr(0, i + 1));
	}

	return(L"");
}

inline std::string GetPath(const std::string& s)
{

	size_t i = s.rfind('/', s.length());
	if (i != std::string::npos)
	{
		return(s.substr(0, i + 1));
	}

	return("");
}