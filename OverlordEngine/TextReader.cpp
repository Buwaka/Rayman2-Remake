#include "stdafx.h"
#include "TextReader.h"




TextReader::TextReader() : m_Exists(false)
{
}


std::string TextReader::ReadLine()
{
	if (eof())
		return "";

	auto temp = m_Lines[m_Counter];

	m_Counter++;

	return temp;
}

void TextReader::Open(std::string Textfile)
{
	std::ifstream in(Textfile, std::ifstream::in | std::ifstream::binary);

	if (in.is_open())
	{
		std::string Container;
		in.seekg(0, in.end);
		m_size = (UINT)in.tellg();
		Container.reserve(m_size);
		Container.resize(m_size);
		in.seekg(0, in.beg);
		in.read(&Container[0], m_size);
		in.close();

		m_Lines = tokenize(Container, '\n');

		for (auto& line : m_Lines)
		{
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		}

		m_Counter = 0;

		m_Exists = true;
	}
	else
	{
		std::wstringstream ss;
		ss << L"BinaryReader::Open> Failed to open the file!\nTextFile: ";
		ss << To_wstringSimple(Textfile);
		Logger::LogWarning(ss.str());
		Logger::LogFormat(LogLevel::Warning, L"TextReader::Open > failed to open file ", Textfile.c_str());

	}
}

bool TextReader::eof()
{
	if (m_Counter >= m_Lines.size() )
	{
		return true;
	}
	return false;
}







TextReaderW::TextReaderW()
{
}


TextReaderW::~TextReaderW()
{
	Close();
}

int TextReaderW::ReadLine(std::wstring& out)
{
	if (m_pReader->eof())
	{
		return -1;
	}


	out = L"";
	std::getline(*m_pReader, out);
	return out.size();
}

void TextReaderW::Open(std::wstring Textfile)
{
	Close();

	auto temp = new std::wifstream();
	temp->open(Textfile, std::ios::in);
	if (temp->is_open())
	{
		m_pReader = temp;
		m_Exists = true;
	}
	else
	{
		std::wstringstream ss;
		ss << L"BinaryReader::Open> Failed to open the file!\nTextFile: ";
		ss << Textfile;
		Logger::LogWarning(ss.str());
		Close();
	}
}

void TextReaderW::Close()
{
	if (m_pReader)
	{
		if (m_pReader->is_open())
			m_pReader->close();
	}

	SafeDelete(m_pReader);
	m_Exists = false;
}
