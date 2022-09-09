#ifndef RENDERING_REFERENCES_H_
#define RENDERING_REFERENCES_H_

#include <string>

namespace Rhythmic
{
	namespace RenderManager
	{
		void RemoveByReference(const std::string &reference, int type);
	}

	template<typename T, typename S, int type>
	class DataReference
	{
	public:
		T *data;
		T empty;
		bool valid;

		inline DataReference() :
			data(0),
			reference(0),
			referenceCounter(0),
			destroyed(false),
			valid(false)
		{ }

		inline DataReference(T *data, S *reference, unsigned int *referenceCounter) :
			data(data),
			reference(reference),
			referenceCounter(referenceCounter),
			destroyed(false)
		{
			if (referenceCounter)
			{
				(*referenceCounter)++;
				valid = true;
			}
		}

		inline DataReference(const DataReference &ref) :
			data(ref.data),
			reference(ref.reference),
			referenceCounter(ref.referenceCounter),
			destroyed(false),
			valid(ref.valid)
		{
			if (referenceCounter) (*referenceCounter)++;
		}

		inline ~DataReference()
		{
			Dispose();
		}

		inline void Dispose()
		{
			if (referenceCounter && !destroyed)
			{
				valid = false;
				destroyed = true;
				(*referenceCounter)--;
				if ((*referenceCounter) == 0)
					RenderManager::RemoveByReference(*reference, type);
			}
		}

		inline void operator=(const DataReference &ref)
		{
			Dispose();
			data = ref.data;
			reference = ref.reference;
			referenceCounter = ref.referenceCounter;
			destroyed = false;
			valid = ref.valid;
			if (referenceCounter)
			{
				(*referenceCounter)++;
			}
		}

		inline T *operator->() { if (data) return data; return &empty; }
	private:
		S *reference;
		unsigned int *referenceCounter;
		bool destroyed;
	};
}

#endif

