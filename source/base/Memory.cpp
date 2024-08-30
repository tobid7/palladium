#include <cstdlib>
#include <map>
#include <pd/base/Memory.hpp>
#include <pd/internal_db.hpp>

static Palladium::Memory::memory_metrics metrics;

void *operator new(size_t size) {
  void *ptr = malloc(size);
  if (pd_flags & PDFlags_MemTrack) metrics.t_TotalAllocated += size;
  return ptr;
}

void operator delete(void *memory, size_t size) {
  if (pd_flags & PDFlags_MemTrack) metrics.t_TotalFreed += size;
  free(memory);
}

int allocations = 0;
int total_size = 0;
std::map<void *, size_t> sizes;

void *operator new[](size_t size) {
  void *ptr = malloc(size);
  if (pd_flags & PDFlags_MemTrack) {
    allocations++;
    total_size += size;
    sizes[ptr] = size;
    metrics.t_TotalAllocated += size;
  }

  return ptr;
}

void operator delete[](void *ptr) {
  if (pd_flags & PDFlags_MemTrack) {
    allocations--;
    total_size -= sizes[ptr];
    metrics.t_TotalFreed += sizes[ptr];
    sizes.erase(ptr);
  }
  free(ptr);
}

namespace Palladium {

namespace Memory {

size_t GetTotalAllocated() { return metrics.t_TotalAllocated; }

size_t GetTotalFreed() { return metrics.t_TotalFreed; }

size_t GetCurrent() { return metrics.t_CurrentlyAllocated(); }
}  // namespace Memory
}  // namespace Palladium
