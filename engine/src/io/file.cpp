#include <wge/io/file.hpp>

#include <fstream>
#include <vector>
#include <memory>

namespace WGE
{
	namespace IO
	{
		struct native_File
		{
			std::vector<char> m_contents;
		};
		struct native_FSI
		{
			std::ifstream m_stream;
		};

		File::File() :
			m_data(std::make_unique<native_File>())
		{ }
		File::~File()
		{ }

		void File::Open(const std::string &file)
		{
			std::ifstream inFile(file, std::ifstream::binary);
			m_data->m_contents.clear();
			
			if (!inFile.is_open())
				return;

			// Resize contents to the file size
			inFile.seekg(0, std::ios::end);
			m_data->m_contents.resize(inFile.tellg());
			inFile.seekg(0, std::ios::beg);

			inFile.read(&m_data->m_contents[0], m_data->m_contents.size());

			inFile.close();
		}
		void File::Save(const std::string &file)
		{
			std::ofstream outFile(file, std::ofstream::binary);

			// Can't write to an unopened file. 
			// This should never happen as ofstream will create the file
			// as long as the file created is in a valid directory
			if (!outFile.is_open())
				return;

			outFile.clear(); // Clear the contents inside the file

			outFile.write(&m_data->m_contents[0], m_data->m_contents.size());

			outFile.close();
		}

		const std::vector<char> *File::GetContents()
		{
			return &m_data->m_contents;
		}
		void File::SetContents(const std::vector<char> &contents)
		{
			m_data->m_contents = contents;
		}
		size_t File::GetSize()
		{
			return m_data->m_contents.size();
		}


		// Input File Stream
		FileStreamIn::FileStreamIn() :
			m_data(std::make_unique<native_FSI>())
		{ }
		FileStreamIn::~FileStreamIn()
		{ }

		void FileStreamIn::Open(const std::string &file)
		{
			m_data->m_stream.close();

			m_data->m_stream.open(file, std::ifstream::binary);
			if (!m_data->m_stream.is_open())
			{
				return;
			}
		}
		void FileStreamIn::Close()
		{
			m_data->m_stream.close();
		}

		void FileStreamIn::Seek(size_t pos)
		{
			if (m_data->m_stream.is_open())
				m_data->m_stream.seekg(pos);
		}
		size_t FileStreamIn::GetHeadPos()
		{
			if (m_data->m_stream.is_open())
				return m_data->m_stream.tellg();
			return 0;
		}

		void FileStreamIn::ReadContents(char *buffer, size_t size)
		{
			//m_data->m_stream.readsome()
		}
		size_t FileStreamIn::GetSize()
		{
			if (!m_data->m_stream.is_open())
				return 0;

			auto currentPos = m_data->m_stream.tellg();

			m_data->m_stream.seekg(0, std::ios::end);
			size_t size = m_data->m_stream.tellg();

			m_data->m_stream.seekg(currentPos);

			return size;
		}
	}
}