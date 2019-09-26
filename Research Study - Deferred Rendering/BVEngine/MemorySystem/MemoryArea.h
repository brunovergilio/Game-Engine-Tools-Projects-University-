#ifndef MEMORYAREA_H
#define MEMORYAREA_H

namespace BVMemory
{
	class MemoryArea
	{
	public:
		MemoryArea();
		~MemoryArea();

		void Create(const size_t size);
		void Destroy();

		inline char *GetStart() const { return m_pStart; };
		inline char *GetEnd() const { return m_pEnd; }

	private:
		MemoryArea(const MemoryArea & other) = delete;
		MemoryArea(MemoryArea && other) = delete;
		MemoryArea & operator = (const MemoryArea & other) = delete;
		MemoryArea & operator = (MemoryArea && other) = delete;

	private:
		char *m_pStart;
		char *m_pEnd;
	};
}
#endif