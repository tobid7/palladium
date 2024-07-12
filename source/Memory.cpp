#include <cstdlib>
#include <map>
#include <pd/Memory.hpp>

static Palladium::Memory::memory_metrics metrics;

bool pdi_enable_memtrack;

void *operator new(size_t size) {
  void *ptr = malloc(size);
  if (pdi_enable_memtrack) metrics.t_TotalAllocated += size;
  return ptr;
}

void operator delete(void *memory, size_t size) {
  if (pdi_enable_memtrack) metrics.t_TotalFreed += size;
  free(memory);
}

int allocations = 0;
int total_size = 0;
std::map<void *, size_t> sizes;

void *operator new[](size_t size) {
  void *ptr = malloc(size);
  if (pdi_enable_memtrack) {
    allocations++;
    total_size += size;
    sizes[ptr] = size;
    metrics.t_TotalAllocated += size;
  }

  return ptr;
}

void operator delete[](void *ptr) {
  if (pdi_enable_memtrack) {
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
