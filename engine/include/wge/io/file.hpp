#ifndef WGE_FILE_HPP_
#define WGE_FILE_HPP_

#include <string>

#include <memory>
#include <vector>

namespace WGE
{
	namespace IO
	{
		struct native_File;
		struct native_FSI;

		/*
		Reads an entire file into memory
		*/
		class File
		{
		public:
			File();
			~File();

			void Open(const std::string &file);
			//void OpenAsync(const std::string &file); Implement later
			void Save(const std::string &file);

			const std::vector<char> *GetContents();		// Returns contents inside the file. Returns NULL if no contents have been read, or invalid file
			void SetContents(const std::vector<char> &contents); // Sets the contents inside the file
			size_t GetSize();				// Returns size of the file
		private:
			std::unique_ptr<native_File> m_data;
		};

		/*
		Opens a file stream
		*/
		class FileStreamIn
		{
		public:
			FileStreamIn();
			~FileStreamIn();

			void Open(const std::string &file);
			void Close();

			void Seek(size_t pos);													// Sets the seek head position
			size_t GetHeadPos();														// Returns the current seek head position

			void ReadContents(char *buffer, size_t size);		// Reads contents of the stream
			size_t GetSize();																// Returns the size of the file
		private:
			std::unique_ptr<native_FSI> m_data;
		};
	}
}

#endif

